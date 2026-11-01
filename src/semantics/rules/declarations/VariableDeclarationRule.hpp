#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class VariableDeclarationRule : public StatementRule<VariableDeclaration> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(VariableDeclaration& node, const Context& ctx) override;
};
