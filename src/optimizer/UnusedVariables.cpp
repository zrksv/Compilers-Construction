#include "UnusedVariables.hpp"
#include "OptimizerUtils.hpp"
#include <algorithm>

bool UnusedVariableEliminator::run(Program* program) {
    if (!program) return false;

    m_changed = false;
    for (auto& declaration : program->declarations) {
        if (auto* routine = dynamic_cast<RoutineDeclaration*>(declaration.get())) {
            m_used.clear();
            eliminate_in_statements(routine->body);
        }
    }
    return m_changed;
}

void UnusedVariableEliminator::eliminate_in_statements(
    std::vector<std::unique_ptr<AstNode>>& statements) {
    for (auto it = statements.rbegin(); it != statements.rend(); ++it) {
        auto& statement = *it;
        if (auto* variable = dynamic_cast<VariableDeclaration*>(statement.get())) {
            const auto* symbol = m_model.variable_of(variable);
            // Creating an aggregate can fail when linear memory is exhausted.
            if (symbol && symbol->type && !TypeCompatibility::is_aggregate(symbol->type) &&
                !m_used.contains(symbol) &&
                optimizer_utils::can_discard_evaluation(variable->expression.get(), m_model)) {
                optimizer_utils::forget_subtree(m_model, variable);
                statement.reset();
                m_changed = true;
                continue;
            }
        }

        if (auto* branch = dynamic_cast<IfStatement*>(statement.get())) {
            eliminate_in_statements(branch->then_body);
            eliminate_in_statements(branch->else_body);
            collect_uses(branch->condition.get());
        } else if (auto* loop = dynamic_cast<WhileStatement*>(statement.get())) {
            eliminate_in_statements(loop->body);
            collect_uses(loop->condition.get());
        } else if (auto* loop = dynamic_cast<ForStatement*>(statement.get())) {
            eliminate_in_statements(loop->body);
            collect_uses(loop->range_start.get());
            collect_uses(loop->range_end.get());
        } else {
            collect_uses(statement.get());
        }
    }

    std::erase_if(statements, [](const auto& statement) { return !statement; });
}

void UnusedVariableEliminator::collect_uses(const AstNode* node) {
    if (const auto* identifier = dynamic_cast<const Identifier*>(node)) {
        if (const auto* symbol = m_model.variable_of(identifier)) {
            m_used.insert(symbol);
        }
    }
    optimizer_utils::for_each_child(node, [this](const AstNode* child) { collect_uses(child); });
}
