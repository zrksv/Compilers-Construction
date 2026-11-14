#pragma once
#include <memory>
#include <unordered_set>
#include <vector>
#include "ast/AST.hpp"
#include "../semantics/SemanticModel.hpp"

class UnusedVariableEliminator {
public:
    explicit UnusedVariableEliminator(SemanticModel& model) : m_model(model) {}

    bool run(Program* program);

private:
    void eliminate_in_statements(std::vector<std::unique_ptr<AstNode>>& statements);
    void collect_uses(const AstNode* node);
    std::unordered_set<const VariableSymbol*> m_used;
    SemanticModel& m_model;
    bool m_changed = false;
};
