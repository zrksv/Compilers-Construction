#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class FieldAccessRule : public ExpressionRule<FieldAccessExpr> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(FieldAccessExpr& node, const Context& ctx) override {
        auto obj_res = m_dispatcher.expression(node.object.get(), ctx);
        if (obj_res.is_error()) {
            return ExprResult::error();
        }

        auto u = obj_res.type->unalias();

        if (u->is_array() && node.field_name == "size") {
            return ExprResult::readonly(PrimitiveType::get_integer(), "Array size is read-only");
        }

        if (u->is_record()) {
            auto rec = std::static_pointer_cast<RecordType>(u);
            const auto* field = rec->find_field(node.field_name);
            if (!field) {
                error(node.location, "Record '" + rec->name + "' has no field named '" + node.field_name + "'");
                return ExprResult::error();
            }

            if (obj_res.access == Access::ReadOnly) {
                return ExprResult::readonly(field->type, obj_res.readonly_reason);
            }
            return ExprResult::writable(field->type);
        }

        error(node.location, "Cannot access field on non-record type: " + obj_res.type->to_string());
        return ExprResult::error();
    }
};
