#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class PrintRule final : public StatementRule<PrintStatement> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(PrintStatement& node, const Context& ctx) override;
};
