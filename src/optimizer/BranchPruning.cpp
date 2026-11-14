#include "BranchPruning.hpp"
#include "OptimizerUtils.hpp"
#include <algorithm>

namespace {

// Match the semantic analyzer's structural check for returns in a block.
bool guarantees_return(const AstNode* statement) {
    if (dynamic_cast<const ReturnStatement*>(statement)) return true;
    if (const auto* branch = dynamic_cast<const IfStatement*>(statement)) {
        const auto returns = [](const auto& child) { return guarantees_return(child.get()); };
        return std::ranges::any_of(branch->then_body, returns) &&
               std::ranges::any_of(branch->else_body, returns);
    }
    return false;
}

} // namespace

bool BranchPruner::run(Program* program) {
    if (!program) return false;
    m_changed = false;
    prune_statements(program->declarations);
    return m_changed;
}

void BranchPruner::prune_statements(std::vector<std::unique_ptr<AstNode>>& statements) {
    std::vector<std::unique_ptr<AstNode>> result;

    for (auto& statement : statements) {
        if (!statement) continue;

        if (auto* branch = dynamic_cast<IfStatement*>(statement.get())) {
            const auto condition = optimizer_utils::constant_condition(branch->condition.get(), m_model);
            if (condition) {
                if (!prune_constant_if(*branch, *condition, result)) continue;
            } else {
                prune_statements(branch->then_body);
                prune_statements(branch->else_body);
            }
        } else if (auto* loop = dynamic_cast<WhileStatement*>(statement.get())) {
            if (optimizer_utils::constant_condition(loop->condition.get(), m_model) == false) {
                optimizer_utils::forget_subtree(m_model, loop);
                m_changed = true;
                continue;
            }
            prune_statements(loop->body);
        } else if (auto* loop = dynamic_cast<ForStatement*>(statement.get())) {
            prune_statements(loop->body);
        } else if (auto* routine = dynamic_cast<RoutineDeclaration*>(statement.get())) {
            prune_statements(routine->body);
        }

        result.push_back(std::move(statement));
    }

    statements = std::move(result);
}

bool BranchPruner::prune_constant_if(
    IfStatement& branch, bool take_then, std::vector<std::unique_ptr<AstNode>>& result) {
    auto& selected = take_then ? branch.then_body : branch.else_body;
    prune_statements(selected);

    if (!optimizer_utils::declares_names(selected)) {
        for (auto& statement : selected) {
            result.push_back(std::move(statement));
        }
        optimizer_utils::forget_subtree(m_model, &branch);
        m_changed = true;
        return false;
    }

    // Keep both return paths when the selected branch still needs its own scope.
    if (guarantees_return(&branch)) {
        prune_statements(take_then ? branch.else_body : branch.then_body);
        return true;
    }

    if (!take_then) {
        branch.then_body.swap(branch.else_body);
    }

    const auto* literal = dynamic_cast<const BoolLiteral*>(branch.condition.get());
    if (!literal || !literal->value) {
        optimizer_utils::forget_subtree(m_model, branch.condition.get());
        branch.condition = std::make_unique<BoolLiteral>(true, branch.condition->location);
        m_model.set_type(branch.condition.get(), PrimitiveType::get_boolean());
        m_changed = true;
    }

    if (!branch.else_body.empty()) {
        optimizer_utils::forget_all(m_model, branch.else_body);
        branch.else_body.clear();
        m_changed = true;
    }
    return true;
}
