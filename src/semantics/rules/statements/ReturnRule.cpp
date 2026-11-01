#include "rules/statements/ReturnRule.hpp"

StmtResult ReturnRule::analyze(ReturnStatement& node, const Context& ctx) {
    if (!ctx.routine) {
        if (node.expression) {
            m_dispatcher.expression(node.expression.get(), ctx);
        }
        error(node.location, "'return' is allowed only inside a routine");
        return StmtResult{true};
    }

    const TypeRef& result_type = ctx.routine->return_type;
    const std::string& routine_name = ctx.routine->name;

    if (result_type->is_void()) {
        if (node.expression) {
            m_dispatcher.expression(node.expression.get(), ctx);
            error(node.location, "Routine '" + routine_name + "' has no return type and cannot return a value");
        }
        return StmtResult{true};
    }

    if (!node.expression) {
        error(node.location, "Routine '" + routine_name + "' must return a value of type " +
                             result_type->to_string());
        return StmtResult{true};
    }

    ExprResult value = m_dispatcher.expression(node.expression.get(), ctx);
    if (!value.is_error()) {
        convert(result_type, value.type, node.expression.get());
    }
    return StmtResult{true};
}
