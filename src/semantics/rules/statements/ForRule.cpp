#include "rules/statements/ForRule.hpp"

StmtResult ForRule::analyze(ForStatement& node, const Context& ctx) {
    TypeRef iterator_type = PrimitiveType::get_integer();

    if (node.range_end) {
        ExprResult start = m_dispatcher.expression(node.range_start.get(), ctx);
        ExprResult end = m_dispatcher.expression(node.range_end.get(), ctx);
        convert(PrimitiveType::get_integer(), start.type, node.range_start.get());
        convert(PrimitiveType::get_integer(), end.type, node.range_end.get());
    } else {
        ExprResult collection = m_dispatcher.expression(node.range_start.get(), ctx);
        if (collection.is_error()) {
            iterator_type = PrimitiveType::get_error();
        } else if (!collection.type->is_array()) {
            error(node.range_start->location,
                  "'for' without '..' requires an array, got " + collection.type->to_string());
            iterator_type = PrimitiveType::get_error();
        } else {
            iterator_type = std::static_pointer_cast<ArrayType>(collection.type->unalias())->element_type;
        }
    }

    Context loop_ctx = ctx.with_child_scope();
    if (loop_ctx.scope) {
        loop_ctx.scope->declare_variable(VariableSymbol(node.iterator_name, iterator_type, true, node.location));
    }

    m_dispatcher.statement_list(node.body, loop_ctx);
    return StmtResult{};
}
