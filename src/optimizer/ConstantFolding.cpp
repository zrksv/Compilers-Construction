#include "ConstantFolding.hpp"
#include "OptimizerUtils.hpp"
#include <type_traits>

bool ConstantFolder::run(Program* program) {
    if (!program) return false;
    m_changed = false;
    optimize_children(program);
    return m_changed;
}

std::unique_ptr<Expression> ConstantFolder::optimize_expr(std::unique_ptr<Expression> expr) {
    if (!expr || optimizer_utils::is_literal(expr.get())) return expr;

    optimize_children(expr.get());

    std::unique_ptr<Expression> replacement;
    if (auto* binary = dynamic_cast<BinaryExpr*>(expr.get())) {
        const auto* left_boolean = dynamic_cast<const BoolLiteral*>(binary->left.get());
        const bool skips_right = left_boolean &&
            ((binary->op == BinOp::And && !left_boolean->value) ||
             (binary->op == BinOp::Or && left_boolean->value));
        if (skips_right || (optimizer_utils::is_literal(binary->left.get()) &&
                            optimizer_utils::is_literal(binary->right.get()))) {
            const auto value = ConstantEvaluator::evaluate(binary);
            if (value.has_value()) {
                replacement = optimizer_utils::make_literal(value, expr->location);
            }
        }
        if (!replacement) replacement = simplify_binary(*binary);
    } else if (const auto* unary = dynamic_cast<const UnaryExpr*>(expr.get());
               unary && optimizer_utils::is_literal(unary->child.get())) {
        const auto value = ConstantEvaluator::evaluate(unary);
        if (value.has_value()) {
            replacement = optimizer_utils::make_literal(value, expr->location);
        }
    }
    if (!replacement) return expr;

    m_changed = true;
    return optimizer_utils::replace_expr(m_model, std::move(expr), std::move(replacement));
}

std::unique_ptr<Expression> ConstantFolder::simplify_binary(BinaryExpr& expr) {
    const auto* left_integer = dynamic_cast<const IntLiteral*>(expr.left.get());
    const auto* right_integer = dynamic_cast<const IntLiteral*>(expr.right.get());
    const auto* left_boolean = dynamic_cast<const BoolLiteral*>(expr.left.get());
    const auto* right_boolean = dynamic_cast<const BoolLiteral*>(expr.right.get());
    const auto result_type = m_model.type_of(&expr);
    const bool integer_result = result_type && result_type->is_integer();

    switch (expr.op) {
        case BinOp::Plus:
            if (!integer_result) break;
            if (right_integer && right_integer->value == 0) return std::move(expr.left);
            if (left_integer && left_integer->value == 0) return std::move(expr.right);
            break;

        case BinOp::Minus:
            if (integer_result && right_integer && right_integer->value == 0) {
                return std::move(expr.left);
            }
            break;

        case BinOp::Mul:
            if (!integer_result) break;
            if (right_integer && right_integer->value == 1) return std::move(expr.left);
            if (left_integer && left_integer->value == 1) return std::move(expr.right);
            if ((right_integer && right_integer->value == 0 &&
                 optimizer_utils::can_discard_evaluation(expr.left.get(), m_model)) ||
                (left_integer && left_integer->value == 0 &&
                 optimizer_utils::can_discard_evaluation(expr.right.get(), m_model))) {
                return std::make_unique<IntLiteral>(0, expr.location);
            }
            break;

        case BinOp::Div:
            if (integer_result && right_integer && right_integer->value == 1) {
                return std::move(expr.left);
            }
            break;

        case BinOp::And:
            if (left_boolean && left_boolean->value) return std::move(expr.right);
            if (right_boolean && right_boolean->value) return std::move(expr.left);
            if (right_boolean && !right_boolean->value &&
                optimizer_utils::can_discard_evaluation(expr.left.get(), m_model)) {
                return std::make_unique<BoolLiteral>(false, expr.location);
            }
            break;

        case BinOp::Or:
            if (left_boolean && !left_boolean->value) return std::move(expr.right);
            if (right_boolean && !right_boolean->value) return std::move(expr.left);
            if (right_boolean && right_boolean->value &&
                optimizer_utils::can_discard_evaluation(expr.left.get(), m_model)) {
                return std::make_unique<BoolLiteral>(true, expr.location);
            }
            break;

        default:
            break;
    }

    return nullptr;
}

void ConstantFolder::optimize_children(AstNode* node) {
    optimizer_utils::detail::for_each_child(node, [this](auto& child) {
        using Child = typename std::remove_reference_t<decltype(child)>::element_type;
        if constexpr (std::is_same_v<Child, Expression>) {
            child = optimize_expr(std::move(child));
        } else {
            optimize_children(child.get());
        }
    });
}
