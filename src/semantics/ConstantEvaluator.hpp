#pragma once
#include <cstdint>
#include <optional>
#include "ast/AST.hpp"

enum class ConstantKind {
    None,
    Integer,
    Real,
    Boolean
};

struct ConstantValue {
    ConstantKind kind = ConstantKind::None;
    int64_t int_val = 0;
    double real_val = 0.0;
    bool bool_val = false;

    static ConstantValue integer(int64_t v) {
        ConstantValue cv;
        cv.kind = ConstantKind::Integer;
        cv.int_val = v;
        return cv;
    }

    static ConstantValue real(double v) {
        ConstantValue cv;
        cv.kind = ConstantKind::Real;
        cv.real_val = v;
        return cv;
    }

    static ConstantValue boolean(bool v) {
        ConstantValue cv;
        cv.kind = ConstantKind::Boolean;
        cv.bool_val = v;
        return cv;
    }

    static ConstantValue none() {
        return ConstantValue{};
    }

    bool has_value() const { return kind != ConstantKind::None; }

    bool is_zero() const {
        if (kind == ConstantKind::Integer) return int_val == 0;
        if (kind == ConstantKind::Real) return real_val == 0.0;
        return false;
    }
};

class ConstantEvaluator {
public:
    static ConstantValue evaluate(const Expression* expr);
};
