#include "rules/statements/AssignRule.hpp"

StmtResult AssignRule::analyze(AssignStatement& node, const Context& ctx) {
    ExprResult target = m_dispatcher.expression(node.target.get(), ctx);
    ExprResult value = m_dispatcher.expression(node.value.get(), ctx);

    if (target.is_error() || value.is_error()) {
        return StmtResult{};
    }

    if (target.access == Access::ReadOnly) {
        error(node.target->location, target.readonly_reason);
        return StmtResult{};
    }

    if (target.access == Access::Value) {
        error(node.target->location,
              "Left side of assignment must be a variable, a record field or an array element");
        return StmtResult{};
    }

    convert(target.type, value.type, node.value.get());
    return StmtResult{};
}
