#pragma once
#include <memory>
#include <vector>
#include "ast/AST.hpp"
#include "../semantics/SemanticModel.hpp"

class BranchPruner {
public:
    explicit BranchPruner(SemanticModel& model) : m_model(model) {}

    bool run(Program* program);

private:
    void prune_statements(std::vector<std::unique_ptr<AstNode>>& stmts);
    bool prune_constant_if(IfStatement& branch, bool take_then,
                           std::vector<std::unique_ptr<AstNode>>& result);
    SemanticModel& m_model;
    bool m_changed = false;
};
