#pragma once
#include "Type.hpp"

enum class ConversionKind {
    None,
    IntegerToReal,
    RealToInteger,
    BooleanToInteger,
    BooleanToReal,
    IntegerToBoolean
};

class TypeCompatibility {
public:
    static bool can_assign(const TypeRef& target, const TypeRef& source);
    static ConversionKind conversion(const TypeRef& target, const TypeRef& source);
    static bool is_aggregate(const TypeRef& type);
};
