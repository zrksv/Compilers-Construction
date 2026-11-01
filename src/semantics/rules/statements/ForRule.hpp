#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class ForRule final : public StatementRule<ForStatement> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(ForStatement& node, const Context& ctx) override;
};
