#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class PrimitiveTypeRule : public TypeRule<PrimitiveTypeNode> {
public:
    using TypeRule::TypeRule;

    TypeRef resolve(PrimitiveTypeNode& node, const Context& ctx) override;
};
