#include "DeadCode.hpp"
#include "OptimizerUtils.hpp"

bool DeadCodeEliminator::run(Program* program) {
    if (!program) return false;
    m_changed = false;
    eliminate_in_statements(program->declarations);
    return m_changed;
}

void DeadCodeEliminator::eliminate_in_statements(std::vector<std::unique_ptr<AstNode>>& stmts) {
    std::vector<std::unique_ptr<AstNode>> result;

    for (size_t i = 0; i < stmts.size(); ++i) {
        auto& stmt = stmts[i];
        if (!stmt) continue;

        if (dynamic_cast<ReturnStatement*>(stmt.get())) {
            result.push_back(std::move(stmt));
            if (i + 1 < stmts.size()) m_changed = true;
            break;
        }

        if (auto* ifs = dynamic_cast<IfStatement*>(stmt.get())) {
            eliminate_in_statements(ifs->then_body);
            eliminate_in_statements(ifs->else_body);
            result.push_back(std::move(stmt));

            if (optimizer_utils::ends_with_return(ifs->then_body) &&
                optimizer_utils::ends_with_return(ifs->else_body)) {
                if (i + 1 < stmts.size()) m_changed = true;
                break;
            }
            continue;
        }

        if (auto* r = dynamic_cast<RoutineDeclaration*>(stmt.get())) {
            eliminate_in_statements(r->body);
        } else if (auto* whl = dynamic_cast<WhileStatement*>(stmt.get())) {
            eliminate_in_statements(whl->body);
        } else if (auto* f = dynamic_cast<ForStatement*>(stmt.get())) {
            eliminate_in_statements(f->body);
        }

        result.push_back(std::move(stmt));
    }

    stmts = std::move(result);
}
