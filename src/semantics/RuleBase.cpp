#include "RuleBase.hpp"

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
    if (value_expr) {
        ConversionKind kind = TypeCompatibility::conversion(target, source);
        m_services.model.set_conversion(value_expr, kind);
    }
    return true;
}
