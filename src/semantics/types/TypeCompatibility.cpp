#include "TypeCompatibility.hpp"

bool TypeCompatibility::can_assign(const TypeRef& target, const TypeRef& source) {
    if (!target || !source) return false;
    if (target->is_error() || source->is_error()) return true;

    auto t = target->unalias();
    auto s = source->unalias();

    if (t->equals(*s)) return true;

    if (t->is_integer()) {
        return s->is_integer() || s->is_real() || s->is_boolean();
    }

    if (t->is_real()) {
        return s->is_real() || s->is_integer() || s->is_boolean();
    }

    if (t->is_boolean()) {
        if (s->is_real()) return false;
        return s->is_boolean() || s->is_integer();
    }

    return t->equals(*s);
}

ConversionKind TypeCompatibility::conversion(const TypeRef& target, const TypeRef& source) {
    if (!target || !source) return ConversionKind::None;
    if (target->is_error() || source->is_error()) return ConversionKind::None;

    auto t = target->unalias();
    auto s = source->unalias();

    if (t->equals(*s)) return ConversionKind::None;

    if (t->is_integer()) {
        if (s->is_real()) return ConversionKind::RealToInteger;
        if (s->is_boolean()) return ConversionKind::BooleanToInteger;
        return ConversionKind::None;
    }

    if (t->is_real()) {
        if (s->is_integer()) return ConversionKind::IntegerToReal;
        if (s->is_boolean()) return ConversionKind::BooleanToReal;
        return ConversionKind::None;
    }

    if (t->is_boolean()) {
        if (s->is_integer()) return ConversionKind::IntegerToBoolean;
        return ConversionKind::None;
    }

    return ConversionKind::None;
}

bool TypeCompatibility::is_aggregate(const TypeRef& type) {
    if (!type) return false;
    auto u = type->unalias();
    return u->is_array() || u->is_record();
}
