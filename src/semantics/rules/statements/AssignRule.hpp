#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class AssignRule final : public StatementRule<AssignStatement> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(AssignStatement& node, const Context& ctx) override;
};
