#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class IdentifierRule : public ExpressionRule<Identifier> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(Identifier& node, const Context& ctx) override {
        if (!ctx.scope) {
            return ExprResult::error();
        }

        auto* sym = ctx.scope->lookup_variable(node.name);
        if (!sym) {
            error(node.location, "Variable '" + node.name + "' is not declared");
            return ExprResult::error();
        }

        m_services.model.bind_variable(&node, sym);

        if (sym->is_readonly) {
            return ExprResult::readonly(sym->type, "Variable '" + node.name + "' is read-only");
        }
        return ExprResult::writable(sym->type);
    }
};
