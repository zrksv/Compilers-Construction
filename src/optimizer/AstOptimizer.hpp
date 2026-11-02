#pragma once
#include "ast/AST.hpp"

class AstOptimizer {
public:
    bool optimize(Program* program);

    bool fold_constants(Program* program);
    bool prune_branches(Program* program);
    bool eliminate_dead_code(Program* program);
    bool remove_unused_variables(Program* program);
};
