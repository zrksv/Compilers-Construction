#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

// Rule for resolving named type identifiers and aliases
class IdentifierTypeRule : public TypeRule<IdentifierTypeNode> {
public:
    using TypeRule::TypeRule;

    TypeRef resolve(IdentifierTypeNode& node, const Context& ctx) override {
        if (!ctx.scope) {
            return PrimitiveType::get_error();
        }

        auto* symbol = ctx.scope->lookup_type(node.name);
        if (!symbol) {
            error(node.location, "Type '" + node.name + "' is not declared");
            return PrimitiveType::get_error();
        }

        return symbol->type;
    }
};
