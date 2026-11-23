// Unit tests for the expression, conversion, and call lowering. They inspect
// the generated WAT text and therefore do not need WABT or Node.js.
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "SemanticAnalyzer.hpp"
#include "SourceManager.hpp"
#include "WasmCodegen.hpp"

#include <cassert>
#include <iostream>
#include <string>

namespace {

std::string generate(const std::string& source) {
    SourceManager source_manager(source);
    Lexer lexer(source_manager);
    ParserDriver parser;
    assert(parser.parse(lexer));
    auto* program = dynamic_cast<Program*>(parser.get_ast());
    assert(program);
    SemanticAnalyzer analyzer;
    assert(analyzer.analyze(program));
    WasmCodegen codegen;
    return codegen.generate_wat(*program, analyzer.get_model());
}

void contains(const std::string& wat, const std::string& needle) {
    if (wat.find(needle) == std::string::npos) {
        std::cerr << "expected '" << needle << "' in generated WAT:\n" << wat << '\n';
    }
    assert(wat.find(needle) != std::string::npos);
}

void lacks(const std::string& wat, const std::string& needle) {
    if (wat.find(needle) != std::string::npos) {
        std::cerr << "did not expect '" << needle << "' in generated WAT:\n" << wat << '\n';
    }
    assert(wat.find(needle) == std::string::npos);
}

void before(const std::string& wat, const std::string& first, const std::string& second) {
    const auto first_at = wat.find(first);
    const auto second_at = wat.find(second);
    if (first_at == std::string::npos || second_at == std::string::npos || first_at >= second_at) {
        std::cerr << "expected '" << first << "' before '" << second << "' in:\n" << wat << '\n';
    }
    assert(first_at != std::string::npos && second_at != std::string::npos && first_at < second_at);
}

std::string int_body(const std::string& body) {
    return "routine value() : integer is\n" + body + "\nend\n";
}

std::string bool_body(const std::string& body) {
    return "routine value() : boolean is\n" + body + "\nend\n";
}

} // namespace

// Addition and multiplication use the integer domain and keep both operands.
static void test_integer_arithmetic() {
    const auto wat = generate(int_body("    return 2 + 3 * 4"));
    contains(wat, "i32.const 2");
    contains(wat, "i32.const 3");
    contains(wat, "i32.const 4");
    contains(wat, "i32.mul");
    contains(wat, "i32.add");
    lacks(wat, "f64.add");
}

// Subtraction and multiplication use the expected instructions.
static void test_integer_subtraction() {
    const auto wat = generate(int_body("    return 10 - 2 * 3"));
    contains(wat, "i32.mul");
    contains(wat, "i32.sub");
}

// Real operands select the f64 domain.
static void test_real_arithmetic() {
    const auto wat = generate("routine value() : real is\n    return 2.5 + 1.5\nend\n");
    contains(wat, "f64.const 2.5");
    contains(wat, "f64.const 1.5");
    contains(wat, "f64.add");
}

// A mixed expression converts the integer operand before the real operation.
static void test_mixed_arithmetic_promotes_to_real() {
    const auto wat = generate("routine value() : real is\n    return 1 + 2.5\nend\n");
    contains(wat, "i32.const 1");
    contains(wat, "f64.convert_i32_s");
    contains(wat, "f64.const 2.5");
    contains(wat, "f64.add");
}

// Integer division goes through i64 so INT32_MIN / -1 wraps instead of trapping.
static void test_division_wraps_instead_of_trapping() {
    const auto wat = generate(int_body("    return 7 / 2"));
    contains(wat, "i64.extend_i32_s");
    contains(wat, "i64.div_s");
    contains(wat, "i32.wrap_i64");
}

// Real division uses the native f64 instruction.
static void test_real_division() {
    const auto wat = generate("routine value() : real is\n    return 7.0 / 2.0\nend\n");
    contains(wat, "f64.div");
}

// Modulo is integer-only and lowers to the signed remainder.
static void test_modulo() {
    const auto wat = generate(int_body("    return 7 % 2"));
    contains(wat, "i32.rem_s");
}

// Relational operators select i32 or f64 comparisons from the operand types.
static void test_integer_comparisons() {
    contains(generate(bool_body("    return 1 < 2")), "i32.lt_s");
    contains(generate(bool_body("    return 1 <= 2")), "i32.le_s");
    contains(generate(bool_body("    return 1 > 2")), "i32.gt_s");
    contains(generate(bool_body("    return 1 >= 2")), "i32.ge_s");
}

static void test_real_comparison() {
    contains(generate(bool_body("    return 1.5 < 2.0")), "f64.lt");
}

// Equality supports integers, reals, and mixed operand pairs.
static void test_equality() {
    contains(generate(bool_body("    return 1 = 2")), "i32.eq");
    contains(generate(bool_body("    return 1 /= 2")), "i32.ne");

    const auto mixed = generate(bool_body("    return 1.0 = 1"));
    contains(mixed, "f64.convert_i32_s");
    contains(mixed, "f64.eq");
}

// and/or must short-circuit instead of evaluating both operands eagerly.
static void test_and_short_circuits() {
    const auto wat = generate(bool_body("    return true and false"));
    contains(wat, "if (result i32)");
    contains(wat, "else");
    lacks(wat, "i32.and");
}

static void test_or_short_circuits() {
    const auto wat = generate(bool_body("    return true or false"));
    contains(wat, "if (result i32)");
    contains(wat, "else");
}

// xor evaluates both operands and lowers to i32.xor.
static void test_xor() {
    const auto wat = generate(bool_body("    return true xor false"));
    contains(wat, "i32.xor");
}

// not lowers to i32.eqz.
static void test_not() {
    const auto wat = generate(bool_body("    return not true"));
    contains(wat, "i32.eqz");
}

// Unary minus has separate integer and real forms.
static void test_unary_minus_integer() {
    const auto wat = generate(int_body("    return -5"));
    contains(wat, "i32.const -1");
    contains(wat, "i32.mul");
}

static void test_unary_minus_real() {
    const auto wat = generate("routine value() : real is\n    return -2.5\nend\n");
    contains(wat, "f64.neg");
}

// real -> integer rounds half away from zero before narrowing to i32.
static void test_real_to_integer_rounding() {
    const auto wat = generate(int_body("    var x : integer is 2.5\n    return x"));
    contains(wat, "f64.trunc");
    contains(wat, "f64.copysign");
    contains(wat, "f64.const 0.5");
    contains(wat, "f64.ge");
    contains(wat, "i32.trunc_f64_s");
}

// integer -> boolean checks the value before narrowing it to the boolean range.
static void test_integer_to_boolean_check() {
    const auto wat = generate(bool_body("    var b : boolean is 1\n    return b"));
    contains(wat, "i32.eqz");
}

// integer -> real widens with a single conversion instruction.
static void test_integer_to_real() {
    const auto wat = generate("routine value() : real is\n    var r : real is 1\n    return r\nend\n");
    contains(wat, "f64.convert_i32_s");
}

// Binary operands are evaluated left to right.
static void test_call_order_is_left_to_right() {
    const auto wat = generate(
        "routine first() : integer => 1\n"
        "routine second() : integer => 2\n"
        "routine value() : integer is\n    return first() + second()\nend\n");
    contains(wat, "call $routine0");
    contains(wat, "call $routine1");
    before(wat, "call $routine0", "call $routine1");
}

// A function call used as a statement drops its unused result.
static void test_statement_call_drops_result() {
    const auto wat = generate(
        "routine first() : integer => 1\n"
        "routine value() is\n    first()\nend\n");
    contains(wat, "call $routine0");
    contains(wat, "drop");
}

// print dispatches to the host imports and inserts separators.
static void test_print_uses_host_imports() {
    const auto wat = generate(
        "routine value() is\n    print 1, 2.5, true\nend\n");
    contains(wat, "call $print_i32");
    contains(wat, "call $print_f64");
    contains(wat, "call $print_bool");
    contains(wat, "call $print_space");
    contains(wat, "call $print_newline");
}

int main() {
    test_integer_arithmetic();
    test_integer_subtraction();
    test_real_arithmetic();
    test_mixed_arithmetic_promotes_to_real();
    test_division_wraps_instead_of_trapping();
    test_real_division();
    test_modulo();
    test_integer_comparisons();
    test_real_comparison();
    test_equality();
    test_and_short_circuits();
    test_or_short_circuits();
    test_xor();
    test_not();
    test_unary_minus_integer();
    test_unary_minus_real();
    test_real_to_integer_rounding();
    test_integer_to_boolean_check();
    test_integer_to_real();
    test_call_order_is_left_to_right();
    test_statement_call_drops_result();
    test_print_uses_host_imports();
    std::cout << "expression codegen tests passed\n";
    return 0;
}
