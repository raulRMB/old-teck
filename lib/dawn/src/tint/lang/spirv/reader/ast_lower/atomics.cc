// Copyright 2022 The Dawn & Tint Authors
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "src/tint/lang/spirv/reader/ast_lower/atomics.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "src/tint/lang/core/fluent_types.h"
#include "src/tint/lang/core/type/reference.h"
#include "src/tint/lang/wgsl/program/clone_context.h"
#include "src/tint/lang/wgsl/program/program_builder.h"
#include "src/tint/lang/wgsl/resolver/resolve.h"
#include "src/tint/lang/wgsl/sem/block_statement.h"
#include "src/tint/lang/wgsl/sem/function.h"
#include "src/tint/lang/wgsl/sem/index_accessor_expression.h"
#include "src/tint/lang/wgsl/sem/load.h"
#include "src/tint/lang/wgsl/sem/member_accessor_expression.h"
#include "src/tint/lang/wgsl/sem/statement.h"
#include "src/tint/utils/containers/map.h"
#include "src/tint/utils/containers/unique_vector.h"
#include "src/tint/utils/rtti/switch.h"

using namespace tint::core::number_suffixes;  // NOLINT
using namespace tint::core::fluent_types;     // NOLINT

TINT_INSTANTIATE_TYPEINFO(tint::spirv::reader::Atomics);
TINT_INSTANTIATE_TYPEINFO(tint::spirv::reader::Atomics::Stub);

namespace tint::spirv::reader {

/// PIMPL state for the transform
struct Atomics::State {
  private:
    /// A struct that has been forked because a subset of members were made atomic.
    struct ForkedStruct {
        Symbol name;
        std::unordered_set<size_t> atomic_members;
    };

    /// The source program
    const Program& src;
    /// The target program builder
    ProgramBuilder b;
    /// The clone context
    program::CloneContext ctx = {&b, &src, /* auto_clone_symbols */ true};
    std::unordered_map<const core::type::Struct*, ForkedStruct> forked_structs;
    std::unordered_set<const sem::Variable*> atomic_variables;
    UniqueVector<const sem::ValueExpression*, 8> atomic_expressions;

  public:
    /// Constructor
    /// @param program the source program
    explicit State(const Program& program) : src(program) {}

    /// Runs the transform
    /// @returns the new program or SkipTransform if the transform is not required
    ApplyResult Run() {
        bool made_changes = false;

        // Look for stub functions generated by the SPIR-V reader, which are used as placeholders
        // for atomic builtin calls.
        for (auto* fn : ctx.src->AST().Functions()) {
            if (auto* stub = ast::GetAttribute<Stub>(fn->attributes)) {
                auto* sem = ctx.src->Sem().Get(fn);

                for (auto* call : sem->CallSites()) {
                    // The first argument is always the atomic.
                    // The stub passes this by value, whereas the builtin wants a pointer.
                    // Take the address of the atomic argument.
                    auto& args = call->Declaration()->args;
                    auto out_args = ctx.Clone(args);
                    out_args[0] = b.AddressOf(out_args[0]);

                    // Replace all callsites of this stub to a call to the real builtin
                    if (stub->builtin == wgsl::BuiltinFn::kAtomicCompareExchangeWeak) {
                        // atomicCompareExchangeWeak returns a struct, so insert a call to it above
                        // the current statement, and replace the current call with the struct's
                        // `old_value` member.
                        auto* block = call->Stmt()->Block()->Declaration();
                        auto old_value = b.Symbols().New("old_value");
                        auto old_value_decl = b.Decl(b.Let(
                            old_value,
                            b.MemberAccessor(b.Call(wgsl::str(stub->builtin), std::move(out_args)),
                                             "old_value")));
                        ctx.InsertBefore(block->statements, call->Stmt()->Declaration(),
                                         old_value_decl);
                        ctx.Replace(call->Declaration(), b.Expr(old_value));
                    } else {
                        ctx.Replace(call->Declaration(),
                                    b.Call(wgsl::str(stub->builtin), std::move(out_args)));
                    }

                    // Keep track of this expression. We'll need to modify the root identifier /
                    // structure to be atomic.
                    atomic_expressions.Add(ctx.src->Sem().GetVal(args[0]));
                }

                // Remove the stub from the output program
                ctx.Remove(ctx.src->AST().GlobalDeclarations(), fn);
                made_changes = true;
            }
        }

        if (!made_changes) {
            return SkipTransform;
        }

        // Transform all variables and structure members that were used in atomic operations as
        // atomic types. This propagates up originating expression chains.
        ProcessAtomicExpressions();

        // If we need to change structure members, then fork them.
        if (!forked_structs.empty()) {
            ctx.ReplaceAll([&](const ast::Struct* str) {
                // Is `str` a structure we need to fork?
                auto* str_ty = ctx.src->Sem().Get(str);
                if (auto it = forked_structs.find(str_ty); it != forked_structs.end()) {
                    const auto& forked = it->second;

                    // Re-create the structure swapping in the atomic-flavoured members
                    Vector<const ast::StructMember*, 8> members;
                    members.Reserve(str->members.Length());
                    for (size_t i = 0; i < str->members.Length(); i++) {
                        auto* member = str->members[i];
                        if (forked.atomic_members.count(i)) {
                            auto type = AtomicTypeFor(ctx.src->Sem().Get(member)->Type());
                            auto name = member->name->symbol.Name();
                            members.Push(b.Member(name, type, ctx.Clone(member->attributes)));
                        } else {
                            members.Push(ctx.Clone(member));
                        }
                    }
                    b.Structure(forked.name, std::move(members));
                }
                return nullptr;
            });
        }

        // Replace assignments and decls from atomic variables with atomicLoads, and assignments to
        // atomic variables with atomicStores.
        ReplaceLoadsAndStores();

        ctx.Clone();
        return resolver::Resolve(b);
    }

  private:
    ForkedStruct& Fork(const core::type::Struct* str) {
        auto& forked = forked_structs[str];
        if (!forked.name.IsValid()) {
            forked.name = b.Symbols().New(str->Name().Name() + "_atomic");
        }
        return forked;
    }

    void ProcessAtomicExpressions() {
        for (size_t i = 0; i < atomic_expressions.Length(); i++) {
            Switch(
                atomic_expressions[i]->UnwrapLoad(),  //
                [&](const sem::VariableUser* user) {
                    auto* v = user->Variable()->Declaration();
                    if (v->type && atomic_variables.emplace(user->Variable()).second) {
                        ctx.Replace(v->type.expr, b.Expr(AtomicTypeFor(user->Variable()->Type())));
                    }
                    if (auto* ctor = user->Variable()->Initializer()) {
                        atomic_expressions.Add(ctor);
                    }
                },
                [&](const sem::StructMemberAccess* access) {
                    // Fork the struct (the first time) and mark member(s) that need to be made
                    // atomic.
                    auto* member = access->Member();
                    Fork(member->Struct()).atomic_members.emplace(member->Index());
                    atomic_expressions.Add(access->Object());
                },
                [&](const sem::IndexAccessorExpression* index) {
                    atomic_expressions.Add(index->Object());
                },
                [&](const sem::ValueExpression* e) {
                    if (auto* unary = e->Declaration()->As<ast::UnaryOpExpression>()) {
                        atomic_expressions.Add(ctx.src->Sem().GetVal(unary->expr));
                    }
                });
        }
    }

    ast::Type AtomicTypeFor(const core::type::Type* ty) {
        return Switch(
            ty,  //
            [&](const core::type::I32*) { return b.ty.atomic(CreateASTTypeFor(ctx, ty)); },
            [&](const core::type::U32*) { return b.ty.atomic(CreateASTTypeFor(ctx, ty)); },
            [&](const core::type::Struct* str) { return b.ty(Fork(str).name); },
            [&](const core::type::Array* arr) {
                if (arr->Count()->Is<core::type::RuntimeArrayCount>()) {
                    return b.ty.array(AtomicTypeFor(arr->ElemType()));
                }
                auto count = arr->ConstantCount();
                if (!count) {
                    ctx.dst->Diagnostics().add_error(
                        diag::System::Transform,
                        "the Atomics transform does not currently support array counts that "
                        "use override values");
                    count = 1;
                }
                return b.ty.array(AtomicTypeFor(arr->ElemType()), u32(count.value()));
            },
            [&](const core::type::Pointer* ptr) {
                return b.ty.ptr(ptr->AddressSpace(), AtomicTypeFor(ptr->StoreType()),
                                ptr->Access());
            },
            [&](const core::type::Reference* ref) { return AtomicTypeFor(ref->StoreType()); },  //
            TINT_ICE_ON_NO_MATCH);
    }

    void ReplaceLoadsAndStores() {
        // Returns true if 'e' is a reference to an atomic variable or struct member
        auto is_ref_to_atomic_var = [&](const sem::ValueExpression* e) {
            if (tint::Is<core::type::Reference>(e->Type()) && e->RootIdentifier() &&
                (atomic_variables.count(e->RootIdentifier()) != 0)) {
                // If it's a struct member, make sure it's one we marked as atomic
                if (auto* ma = e->As<sem::StructMemberAccess>()) {
                    auto it = forked_structs.find(ma->Member()->Struct());
                    if (it != forked_structs.end()) {
                        auto& forked = it->second;
                        return forked.atomic_members.count(ma->Member()->Index()) != 0;
                    }
                }
                return true;
            }
            return false;
        };

        // Look for loads and stores of atomic variables we've collected so far, and replace them
        // with atomicLoad and atomicStore.
        for (auto* node : ctx.src->ASTNodes().Objects()) {
            if (auto* load = ctx.src->Sem().Get<sem::Load>(node)) {
                if (is_ref_to_atomic_var(load->Reference())) {
                    ctx.Replace(load->Reference()->Declaration(), [=] {
                        auto* expr = ctx.CloneWithoutTransform(load->Reference()->Declaration());
                        return b.Call(wgsl::BuiltinFn::kAtomicLoad, b.AddressOf(expr));
                    });
                }
            } else if (auto* assign = node->As<ast::AssignmentStatement>()) {
                auto* sem_lhs = ctx.src->Sem().GetVal(assign->lhs);
                if (is_ref_to_atomic_var(sem_lhs)) {
                    ctx.Replace(assign, [=] {
                        auto* lhs = ctx.CloneWithoutTransform(assign->lhs);
                        auto* rhs = ctx.CloneWithoutTransform(assign->rhs);
                        auto* call = b.Call(wgsl::BuiltinFn::kAtomicStore, b.AddressOf(lhs), rhs);
                        return b.CallStmt(call);
                    });
                }
            }
        }
    }
};

Atomics::Atomics() = default;
Atomics::~Atomics() = default;

Atomics::Stub::Stub(GenerationID pid, ast::NodeID nid, wgsl::BuiltinFn b)
    : Base(pid, nid, tint::Empty), builtin(b) {}
Atomics::Stub::~Stub() = default;
std::string Atomics::Stub::InternalName() const {
    return "@internal(spirv-atomic " + std::string(wgsl::str(builtin)) + ")";
}

const Atomics::Stub* Atomics::Stub::Clone(ast::CloneContext& ctx) const {
    return ctx.dst->ASTNodes().Create<Atomics::Stub>(ctx.dst->ID(), ctx.dst->AllocateNodeID(),
                                                     builtin);
}

ast::transform::Transform::ApplyResult Atomics::Apply(const Program& src,
                                                      const ast::transform::DataMap&,
                                                      ast::transform::DataMap&) const {
    return State{src}.Run();
}

}  // namespace tint::spirv::reader
