#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class TypeDeclarationRule : public StatementRule<TypeDeclaration> {
public:
    using StatementRule::StatementRule;

    StmtResult analyze(TypeDeclaration& node, const Context& ctx) override;
};
