#include "rules/statements/IfRule.hpp"

StmtResult IfRule::analyze(IfStatement& node, const Context& ctx) {
    ExprResult condition = m_dispatcher.expression(node.condition.get(), ctx);
    convert(PrimitiveType::get_boolean(), condition.type, node.condition.get());

    StmtResult then_result = m_dispatcher.statement_list(node.then_body, ctx.with_child_scope());
    StmtResult else_result = m_dispatcher.statement_list(node.else_body, ctx.with_child_scope());

    return StmtResult{then_result.returns && else_result.returns};
}
