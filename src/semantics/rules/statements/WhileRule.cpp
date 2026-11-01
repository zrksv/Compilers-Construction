#include "rules/statements/WhileRule.hpp"

StmtResult WhileRule::analyze(WhileStatement& node, const Context& ctx) {
    ExprResult condition = m_dispatcher.expression(node.condition.get(), ctx);
    convert(PrimitiveType::get_boolean(), condition.type, node.condition.get());

    m_dispatcher.statement_list(node.body, ctx.with_child_scope());
    return StmtResult{};
}
