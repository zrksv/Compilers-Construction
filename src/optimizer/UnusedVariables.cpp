#include "UnusedVariables.hpp"
#include "OptimizerUtils.hpp"

bool UnusedVariableEliminator::run(Program* program) {
    if (!program) return false;
    m_changed = false;
    eliminate_in_statements(program->declarations, false);
    return m_changed;
}

void UnusedVariableEliminator::eliminate_in_statements(std::vector<std::unique_ptr<AstNode>>& stmts, bool in_routine) {
    std::vector<std::unique_ptr<AstNode>> result;

    for (size_t i = 0; i < stmts.size(); ++i) {
        auto& stmt = stmts[i];
        if (!stmt) continue;

        if (auto* var = dynamic_cast<VariableDeclaration*>(stmt.get())) {
            if (in_routine && optimizer_utils::creation_is_pure(var->type.get(), m_model) &&
                optimizer_utils::is_pure(var->expression.get(), m_model)) {
                bool used = false;
                for (size_t j = i + 1; j < stmts.size(); ++j) {
                    if (optimizer_utils::contains_identifier(stmts[j].get(), var->name)) {
                        used = true;
                        break;
                    }
                }
                if (!used) {
                    m_changed = true;
                    optimizer_utils::forget_subtree(m_model, var);
                    continue;
                }
            }
        }

        if (auto* r = dynamic_cast<RoutineDeclaration*>(stmt.get())) {
            eliminate_in_statements(r->body, true);
        } else if (auto* ifs = dynamic_cast<IfStatement*>(stmt.get())) {
            eliminate_in_statements(ifs->then_body, in_routine);
            eliminate_in_statements(ifs->else_body, in_routine);
        } else if (auto* whl = dynamic_cast<WhileStatement*>(stmt.get())) {
            eliminate_in_statements(whl->body, in_routine);
        } else if (auto* f = dynamic_cast<ForStatement*>(stmt.get())) {
            eliminate_in_statements(f->body, in_routine);
        }

        result.push_back(std::move(stmt));
    }

    stmts = std::move(result);
}
