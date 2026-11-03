#include "BranchPruning.hpp"
#include "OptimizerUtils.hpp"
#include "../semantics/ConstantEvaluator.hpp"

bool BranchPruner::run(Program* program) {
    if (!program) return false;
    m_changed = false;
    prune_statements(program->declarations);
    return m_changed;
}

void BranchPruner::prune_statements(std::vector<std::unique_ptr<AstNode>>& stmts) {
    std::vector<std::unique_ptr<AstNode>> result;

    for (auto& stmt : stmts) {
        if (!stmt) continue;

        if (auto* ifs = dynamic_cast<IfStatement*>(stmt.get())) {
            auto cond = ConstantEvaluator::evaluate(ifs->condition.get());
            if (cond.has_value() && cond.kind == ConstantKind::Boolean) {
                if (!cond.bool_val) {
                    ifs->then_body.swap(ifs->else_body);
                }
                if (!optimizer_utils::declares_names(ifs->then_body)) {
                    m_changed = true;
                    std::vector<std::unique_ptr<AstNode>> branch;
                    branch.swap(ifs->then_body);
                    optimizer_utils::forget_subtree(m_model, ifs);
                    prune_statements(branch);
                    for (auto& s : branch) result.push_back(std::move(s));
                    continue;
                }
                if (!cond.bool_val) {
                    m_changed = true;
                    auto always_true = std::make_unique<BoolLiteral>(true, ifs->condition->location);
                    ifs->condition = optimizer_utils::replace_expr(m_model, std::move(ifs->condition), std::move(always_true));
                }
                if (!ifs->else_body.empty()) {
                    m_changed = true;
                    optimizer_utils::forget_all(m_model, ifs->else_body);
                    ifs->else_body.clear();
                }
            }
            prune_statements(ifs->then_body);
            prune_statements(ifs->else_body);
            result.push_back(std::move(stmt));
            continue;
        }

        if (auto* whl = dynamic_cast<WhileStatement*>(stmt.get())) {
            auto cond = ConstantEvaluator::evaluate(whl->condition.get());
            if (cond.has_value() && cond.kind == ConstantKind::Boolean && !cond.bool_val) {
                m_changed = true;
                optimizer_utils::forget_subtree(m_model, whl);
                continue;
            }
            prune_statements(whl->body);
            result.push_back(std::move(stmt));
            continue;
        }

        if (auto* r = dynamic_cast<RoutineDeclaration*>(stmt.get())) {
            prune_statements(r->body);
        } else if (auto* f = dynamic_cast<ForStatement*>(stmt.get())) {
            prune_statements(f->body);
        }

        result.push_back(std::move(stmt));
    }

    stmts = std::move(result);
}
