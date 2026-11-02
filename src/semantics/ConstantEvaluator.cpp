#include "ConstantEvaluator.hpp"

ConstantValue ConstantEvaluator::evaluate(const Expression* expr) {
    if (!expr) {
        return ConstantValue::none();
    }

    if (const auto* lit = dynamic_cast<const IntLiteral*>(expr)) {
        return ConstantValue::integer(lit->value);
    }

    if (const auto* lit = dynamic_cast<const RealLiteral*>(expr)) {
        return ConstantValue::real(lit->value);
    }

    if (const auto* lit = dynamic_cast<const BoolLiteral*>(expr)) {
        return ConstantValue::boolean(lit->value);
    }

    if (const auto* un = dynamic_cast<const UnaryExpr*>(expr)) {
        auto child = evaluate(un->child.get());
        if (!child.has_value()) {
            return ConstantValue::none();
        }

        switch (un->op) {
            case UnaryOp::Plus:
                if (child.kind == ConstantKind::Integer || child.kind == ConstantKind::Real) {
                    return child;
                }
                return ConstantValue::none();

            case UnaryOp::Minus:
                if (child.kind == ConstantKind::Integer) {
                    return ConstantValue::integer(-child.int_val);
                }
                if (child.kind == ConstantKind::Real) {
                    return ConstantValue::real(-child.real_val);
                }
                return ConstantValue::none();

            case UnaryOp::Not:
                if (child.kind == ConstantKind::Boolean) {
                    return ConstantValue::boolean(!child.bool_val);
                }
                return ConstantValue::none();
        }
        return ConstantValue::none();
    }

    if (const auto* bin = dynamic_cast<const BinaryExpr*>(expr)) {
        auto left = evaluate(bin->left.get());
        auto right = evaluate(bin->right.get());

        if (!left.has_value() || !right.has_value()) {
            return ConstantValue::none();
        }

        // Both operands are Integer
        if (left.kind == ConstantKind::Integer && right.kind == ConstantKind::Integer) {
            switch (bin->op) {
                case BinOp::Plus:
                    return ConstantValue::integer(left.int_val + right.int_val);
                case BinOp::Minus:
                    return ConstantValue::integer(left.int_val - right.int_val);
                case BinOp::Mul:
                    return ConstantValue::integer(left.int_val * right.int_val);
                case BinOp::Div:
                    if (right.int_val == 0) return ConstantValue::none();
                    return ConstantValue::integer(left.int_val / right.int_val);
                case BinOp::Mod:
                    if (right.int_val == 0) return ConstantValue::none();
                    return ConstantValue::integer(left.int_val % right.int_val);
                case BinOp::Equal:
                    return ConstantValue::boolean(left.int_val == right.int_val);
                case BinOp::NotEqual:
                    return ConstantValue::boolean(left.int_val != right.int_val);
                case BinOp::Less:
                    return ConstantValue::boolean(left.int_val < right.int_val);
                case BinOp::LessEqual:
                    return ConstantValue::boolean(left.int_val <= right.int_val);
                case BinOp::Greater:
                    return ConstantValue::boolean(left.int_val > right.int_val);
                case BinOp::GreaterEqual:
                    return ConstantValue::boolean(left.int_val >= right.int_val);
                default:
                    return ConstantValue::none();
            }
        }

        // Numeric operands with at least one Real
        bool left_num = (left.kind == ConstantKind::Integer || left.kind == ConstantKind::Real);
        bool right_num = (right.kind == ConstantKind::Integer || right.kind == ConstantKind::Real);
        if (left_num && right_num && (left.kind == ConstantKind::Real || right.kind == ConstantKind::Real)) {
            double ld = (left.kind == ConstantKind::Real) ? left.real_val : static_cast<double>(left.int_val);
            double rd = (right.kind == ConstantKind::Real) ? right.real_val : static_cast<double>(right.int_val);

            switch (bin->op) {
                case BinOp::Plus:
                    return ConstantValue::real(ld + rd);
                case BinOp::Minus:
                    return ConstantValue::real(ld - rd);
                case BinOp::Mul:
                    return ConstantValue::real(ld * rd);
                case BinOp::Div:
                    if (rd == 0.0) return ConstantValue::none();
                    return ConstantValue::real(ld / rd);
                case BinOp::Equal:
                    return ConstantValue::boolean(ld == rd);
                case BinOp::NotEqual:
                    return ConstantValue::boolean(ld != rd);
                case BinOp::Less:
                    return ConstantValue::boolean(ld < rd);
                case BinOp::LessEqual:
                    return ConstantValue::boolean(ld <= rd);
                case BinOp::Greater:
                    return ConstantValue::boolean(ld > rd);
                case BinOp::GreaterEqual:
                    return ConstantValue::boolean(ld >= rd);
                default:
                    return ConstantValue::none();
            }
        }

        // Both operands are Boolean
        if (left.kind == ConstantKind::Boolean && right.kind == ConstantKind::Boolean) {
            switch (bin->op) {
                case BinOp::And:
                    return ConstantValue::boolean(left.bool_val && right.bool_val);
                case BinOp::Or:
                    return ConstantValue::boolean(left.bool_val || right.bool_val);
                case BinOp::Xor:
                    return ConstantValue::boolean(left.bool_val != right.bool_val);
                case BinOp::Equal:
                    return ConstantValue::boolean(left.bool_val == right.bool_val);
                case BinOp::NotEqual:
                    return ConstantValue::boolean(left.bool_val != right.bool_val);
                default:
                    return ConstantValue::none();
            }
        }

        return ConstantValue::none();
    }

    return ConstantValue::none();
}
