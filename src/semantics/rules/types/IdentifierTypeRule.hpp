#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

// Rule for resolving named type identifiers and aliases
class IdentifierTypeRule : public TypeRule<IdentifierTypeNode> {
public:
    using TypeRule::TypeRule;

    TypeRef resolve(IdentifierTypeNode& node, const Context& ctx) override;
};
