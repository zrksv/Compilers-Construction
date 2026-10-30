#pragma once
#include "RuleBase.hpp"
#include "ast/AST.hpp"

class BinaryRule : public ExpressionRule<BinaryExpr> {
public:
    using ExpressionRule::ExpressionRule;

    ExprResult analyze(BinaryExpr& node, const Context& ctx) override {
        auto left_res = m_dispatcher.expression(node.left.get(), ctx);
        auto right_res = m_dispatcher.expression(node.right.get(), ctx);

        if (left_res.is_error() || right_res.is_error()) {
            return ExprResult::error();
        }

        const auto& left_t = left_res.type;
        const auto& right_t = right_res.type;

        switch (node.op) {
            case BinOp::Plus:
            case BinOp::Minus:
            case BinOp::Mul:
            case BinOp::Div: {
                if (!left_t->is_numeric() || !right_t->is_numeric()) {
                    error(node.location, "Arithmetic operator requires numeric operands");
                    return ExprResult::error();
                }

                if (left_t->is_real() || right_t->is_real()) {
                    if (left_t->is_integer()) {
                        m_services.model.set_conversion(node.left.get(), ConversionKind::IntegerToReal);
                    }
                    if (right_t->is_integer()) {
                        m_services.model.set_conversion(node.right.get(), ConversionKind::IntegerToReal);
                    }
                    return ExprResult::value(PrimitiveType::get_real());
                }
                return ExprResult::value(PrimitiveType::get_integer());
            }

            case BinOp::Mod: {
                if (!left_t->is_integer() || !right_t->is_integer()) {
                    error(node.location, "Operator '%' requires integer operands");
                    return ExprResult::error();
                }
                return ExprResult::value(PrimitiveType::get_integer());
            }

            case BinOp::And:
            case BinOp::Or:
            case BinOp::Xor: {
                if (!left_t->is_boolean() || !right_t->is_boolean()) {
                    error(node.location, "Logical operator requires boolean operands");
                    return ExprResult::error();
                }
                return ExprResult::value(PrimitiveType::get_boolean());
            }

            case BinOp::Less:
            case BinOp::LessEqual:
            case BinOp::Greater:
            case BinOp::GreaterEqual: {
                if (!left_t->is_numeric() || !right_t->is_numeric()) {
                    error(node.location, "Relational operator requires numeric operands");
                    return ExprResult::error();
                }
                if (left_t->is_integer() && right_t->is_real()) {
                    m_services.model.set_conversion(node.left.get(), ConversionKind::IntegerToReal);
                } else if (left_t->is_real() && right_t->is_integer()) {
                    m_services.model.set_conversion(node.right.get(), ConversionKind::IntegerToReal);
                }
                return ExprResult::value(PrimitiveType::get_boolean());
            }

            case BinOp::Equal:
            case BinOp::NotEqual: {
                if (TypeCompatibility::is_aggregate(left_t) || TypeCompatibility::is_aggregate(right_t)) {
                    error(node.location, "Cannot compare aggregate types (arrays and records)");
                    return ExprResult::error();
                }

                if (!TypeCompatibility::can_assign(left_t, right_t) &&
                    !TypeCompatibility::can_assign(right_t, left_t)) {
                    error(node.location, "Cannot compare incompatible types " +
                          left_t->to_string() + " and " + right_t->to_string());
                    return ExprResult::error();
                }

                if (left_t->is_integer() && right_t->is_real()) {
                    m_services.model.set_conversion(node.left.get(), ConversionKind::IntegerToReal);
                } else if (left_t->is_real() && right_t->is_integer()) {
                    m_services.model.set_conversion(node.right.get(), ConversionKind::IntegerToReal);
                }
                return ExprResult::value(PrimitiveType::get_boolean());
            }
        }

        return ExprResult::error();
    }
};
