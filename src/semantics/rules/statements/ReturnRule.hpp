#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class ReturnRule final : public StatementRule<ReturnStatement> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(ReturnStatement& node, const Context& ctx) override;
};
