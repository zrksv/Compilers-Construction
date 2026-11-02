#include "AstOptimizer.hpp"
#include "ConstantFolding.hpp"
#include "BranchPruning.hpp"
#include "DeadCode.hpp"
#include "UnusedVariables.hpp"

bool AstOptimizer::optimize(Program* program) {
    if (!program) return false;
    bool any_changed = false;
    for (int iter = 0; iter < 10; ++iter) {
        bool changed = false;
        changed |= fold_constants(program);
        changed |= prune_branches(program);
        changed |= eliminate_dead_code(program);
        changed |= remove_unused_variables(program);
        if (changed) any_changed = true;
        else break;
    }
    return any_changed;
}

bool AstOptimizer::fold_constants(Program* program) {
    return ConstantFolder{}.run(program);
}

bool AstOptimizer::prune_branches(Program* program) {
    return BranchPruner{}.run(program);
}

bool AstOptimizer::eliminate_dead_code(Program* program) {
    return DeadCodeEliminator{}.run(program);
}

bool AstOptimizer::remove_unused_variables(Program* program) {
    return UnusedVariableEliminator{}.run(program);
}
