    #include "DeadCode.hpp"
#include "OptimizerUtils.hpp"

bool DeadCodeEliminator::run(Program* program) {
    if (!program) return false;
    m_changed = false;
    eliminate_in_statements(program->declarations);
    return m_changed;
}

bool DeadCodeEliminator::eliminate_in_statements(
    std::vector<std::unique_ptr<AstNode>>& statements) {
    for (auto it = statements.begin(); it != statements.end(); ++it) {
        if (!eliminate_in_statement(it->get())) continue;

        const auto unreachable = it + 1;
        if (unreachable != statements.end()) {
            for (auto tail = unreachable; tail != statements.end(); ++tail) {
                optimizer_utils::forget_subtree(m_model, tail->get());
            }
            statements.erase(unreachable, statements.end());
            m_changed = true;
        }
        return true;
    }
    return false;
}

bool DeadCodeEliminator::eliminate_in_statement(AstNode* statement) {
    if (dynamic_cast<ReturnStatement*>(statement)) return true;

    if (auto* branch = dynamic_cast<IfStatement*>(statement)) {
        const bool then_returns = eliminate_in_statements(branch->then_body);
        const bool else_returns = eliminate_in_statements(branch->else_body);
        return then_returns && else_returns;
    }

    if (auto* routine = dynamic_cast<RoutineDeclaration*>(statement)) {
        eliminate_in_statements(routine->body);
    } else if (auto* loop = dynamic_cast<WhileStatement*>(statement)) {
        eliminate_in_statements(loop->body);
    } else if (auto* loop = dynamic_cast<ForStatement*>(statement)) {
        eliminate_in_statements(loop->body);
    }
    return false;
}
