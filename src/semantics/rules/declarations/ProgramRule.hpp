#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class ProgramRule : public StatementRule<Program> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(Program& node, const Context& ctx) override;
};
