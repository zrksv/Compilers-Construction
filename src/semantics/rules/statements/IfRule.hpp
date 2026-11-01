#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class IfRule final : public StatementRule<IfStatement> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(IfStatement& node, const Context& ctx) override;
};
