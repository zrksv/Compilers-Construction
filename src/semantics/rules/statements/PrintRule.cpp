#include "rules/statements/PrintRule.hpp"

StmtResult PrintRule::analyze(PrintStatement& node, const Context& ctx) {
    for (const auto& expr : node.to_print) {
        ExprResult res = m_dispatcher.expression(expr.get(), ctx);
        if (!res.is_error() && TypeCompatibility::is_aggregate(res.type)) {
            error(expr->location, "Cannot print value of aggregate type " + res.type->to_string());
        }
    }
    return StmtResult{};
}
