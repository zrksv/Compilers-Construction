#include "rules/expressions/LiteralRules.hpp"

ExprResult IntLiteralRule::analyze(IntLiteral&, const Context&) {
    return ExprResult::value(PrimitiveType::get_integer());
}

ExprResult RealLiteralRule::analyze(RealLiteral&, const Context&) {
    return ExprResult::value(PrimitiveType::get_real());
}

ExprResult BoolLiteralRule::analyze(BoolLiteral&, const Context&) {
    return ExprResult::value(PrimitiveType::get_boolean());
}
