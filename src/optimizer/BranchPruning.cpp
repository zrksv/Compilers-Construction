#include "BranchPruning.hpp"
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
                m_changed = true;
                auto branch = cond.bool_val ? std::move(ifs->then_body) : std::move(ifs->else_body);
                prune_statements(branch);
                for (auto& s : branch) result.push_back(std::move(s));
                continue;
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
