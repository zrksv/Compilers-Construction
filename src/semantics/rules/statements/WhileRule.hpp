#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class WhileRule final : public StatementRule<WhileStatement> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(WhileStatement& node, const Context& ctx) override;
};
