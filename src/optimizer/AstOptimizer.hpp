#pragma once
#include "ast/AST.hpp"
#include "../semantics/SemanticModel.hpp"

class AstOptimizer {
public:
    explicit AstOptimizer(SemanticModel& model) : m_model(model) {}

    bool optimize(Program* program);

    bool fold_constants(Program* program);
    bool prune_branches(Program* program);
    bool eliminate_dead_code(Program* program);
    bool remove_unused_variables(Program* program);

private:
    SemanticModel& m_model;
};
