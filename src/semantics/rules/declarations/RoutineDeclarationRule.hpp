#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class RoutineDeclarationRule : public StatementRule<RoutineDeclaration> {
private:
    RoutineSymbol* declare_header(RoutineDeclaration& node, const Context& ctx);

    void analyze_body(RoutineDeclaration& node, const Context& ctx, RoutineSymbol* sym);

public:
    using StatementRule::StatementRule;

    StmtResult analyze(RoutineDeclaration& node, const Context& ctx) override;
};
