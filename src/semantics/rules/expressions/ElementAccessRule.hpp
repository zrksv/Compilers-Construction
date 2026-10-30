#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class ElementAccessRule : public ExpressionRule<ElementAccessExpr> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(ElementAccessExpr& node, const Context& ctx) override {
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
        if (arr_res.access == Access::ReadOnly) {
            return ExprResult::readonly(arr->element_type, arr_res.readonly_reason);
        }
        return ExprResult::writable(arr->element_type);
    }
};
