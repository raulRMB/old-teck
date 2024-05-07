// Copyright 2020 The Dawn & Tint Authors
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

#include "src/tint/lang/wgsl/ast/variable.h"
#include "src/tint/lang/wgsl/ast/binding_attribute.h"
#include "src/tint/lang/wgsl/ast/group_attribute.h"
#include "src/tint/lang/wgsl/ast/templated_identifier.h"

TINT_INSTANTIATE_TYPEINFO(tint::ast::Variable);

namespace tint::ast {

Variable::Variable(GenerationID pid,
                   NodeID nid,
                   const Source& src,
                   const Identifier* n,
                   Type ty,
                   const Expression* init,
                   VectorRef<const Attribute*> attrs)
    : Base(pid, nid, src), name(n), type(ty), initializer(init), attributes(std::move(attrs)) {
    TINT_ASSERT(name);
    if (name) {
        TINT_ASSERT(!name->Is<TemplatedIdentifier>());
    }
    TINT_ASSERT_GENERATION_IDS_EQUAL_IF_VALID(initializer, generation_id);
}

Variable::~Variable() = default;

}  // namespace tint::ast
