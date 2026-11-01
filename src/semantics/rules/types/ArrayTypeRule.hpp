#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

// Rule for resolving array types with size or open parameter arrays
class ArrayTypeRule : public TypeRule<ArrayTypeNode> {
public:
    using TypeRule::TypeRule;

    TypeRef resolve(ArrayTypeNode& node, const Context& ctx) override;
};
