#include "RuleBase.hpp"
#include "ConstantEvaluator.hpp"

bool RuleBase::convert(const TypeRef& target, const TypeRef& source, Expression* value_expr, const Location& loc) {
    if (!target || !source) {
        return false;
    }
    if (target->is_error() || source->is_error()) {
        return true;
    }
    if (!TypeCompatibility::can_assign(target, source)) {
        Location err_loc = value_expr ? value_expr->location : loc;
        error(err_loc, "Cannot assign value of type " + source->to_string() + " to " + target->to_string());
        return false;
    }

    if (target->unalias()->is_boolean() && source->unalias()->is_integer() && value_expr) {
        auto val = ConstantEvaluator::evaluate(value_expr);
        if (val.has_value() && val.kind == ConstantKind::Integer) {
            if (val.int_val != 0 && val.int_val != 1) {
                Location err_loc = value_expr->location;
                error(err_loc, "Cannot convert integer value " + std::to_string(val.int_val) +
                               " to boolean (only 0 and 1 are valid)");
                return false;
            }
        }
    }

    if (value_expr) {
        ConversionKind kind = TypeCompatibility::conversion(target, source);
        m_services.model.set_conversion(value_expr, kind);
    }
    return true;
}
