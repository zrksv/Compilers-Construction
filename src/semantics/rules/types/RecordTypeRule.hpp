#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class RecordTypeRule : public TypeRule<RecordTypeNode> {
public:
    using TypeRule::TypeRule;

    TypeRef resolve(RecordTypeNode& node, const Context& ctx) override;
};
