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

#ifndef SRC_TINT_LANG_WGSL_SEM_WHILE_STATEMENT_H_
#define SRC_TINT_LANG_WGSL_SEM_WHILE_STATEMENT_H_

#include "src/tint/lang/wgsl/sem/statement.h"

// Forward declarations
namespace tint::ast {
class WhileStatement;
}  // namespace tint::ast
namespace tint::sem {
class ValueExpression;
}  // namespace tint::sem

namespace tint::sem {

/// Holds semantic information about a while statement
class WhileStatement final : public Castable<WhileStatement, CompoundStatement> {
  public:
    /// Constructor
    /// @param declaration the AST node for this while statement
    /// @param parent the owning statement
    /// @param function the owning function
    WhileStatement(const ast::WhileStatement* declaration,
                   const CompoundStatement* parent,
                   const sem::Function* function);

    /// Destructor
    ~WhileStatement() override;

    /// @returns the AST node
    const ast::WhileStatement* Declaration() const;

    /// @returns the whilecondition expression
    const ValueExpression* Condition() const { return condition_; }

    /// Sets the while condition expression
    /// @param condition the while condition expression
    void SetCondition(const ValueExpression* condition) { condition_ = condition; }

  private:
    const ValueExpression* condition_ = nullptr;
};

}  // namespace tint::sem

#endif  // SRC_TINT_LANG_WGSL_SEM_WHILE_STATEMENT_H_
