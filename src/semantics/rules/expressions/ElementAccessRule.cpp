#include "rules/expressions/ElementAccessRule.hpp"
#include "ConstantEvaluator.hpp"

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

    if (arr->size.has_value()) {
        auto const_idx = ConstantEvaluator::evaluate(node.index.get());
        if (const_idx.has_value() && const_idx.kind == ConstantKind::Integer) {
            int64_t idx_val = const_idx.int_val;
            int64_t sz = static_cast<int64_t>(arr->size.value());
            if (idx_val < 1 || idx_val > sz) {
                Location idx_loc = node.index ? node.index->location : node.location;
                error(idx_loc, "Array index " + std::to_string(idx_val) +
                               " is out of bounds [1.." + std::to_string(sz) + "]");
                return ExprResult::error();
            }
        }
    }

    return ExprResult::writable(arr->element_type);
}
