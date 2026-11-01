#include "rules/expressions/ElementAccessRule.hpp"

ExprResult ElementAccessRule::analyze(ElementAccessExpr& node, const Context& ctx) {
    auto arr_res = m_dispatcher.expression(node.array.get(), ctx);
    auto idx_res = m_dispatcher.expression(node.index.get(), ctx);

    if (arr_res.is_error() || idx_res.is_error()) {
        return ExprResult::error();
    }

    auto u = arr_res.type->unalias();
    if (!u->is_array()) {
        error(node.location, "Indexing target must be an array, got " + arr_res.type->to_string());
        return ExprResult::error();
    }

    if (!idx_res.type->is_integer()) {
        Location idx_loc = node.index ? node.index->location : node.location;
        error(idx_loc, "Array index must be an integer, got " + idx_res.type->to_string());
        return ExprResult::error();
    }

    auto arr = std::static_pointer_cast<ArrayType>(u);
    return ExprResult::writable(arr->element_type);
}
