// End-to-end tests for expression semantics: the fixture is compiled to WASM,
// executed, and checked for values, traps, evaluation order, and exact stdout.
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "SemanticAnalyzer.hpp"
#include "SourceManager.hpp"
#include "WasmCodegen.hpp"
#include "WasmExecution.hpp"
#include "optimizer/AstOptimizer.hpp"

#ifndef IMP_EXPRESSION_SAMPLE
#error IMP_EXPRESSION_SAMPLE must name the expression fixture
#endif

int main(int argc, char** argv) {
    const bool optimize = argc < 2 || std::string(argv[1]) != "--no-opt";
    std::ifstream input(IMP_EXPRESSION_SAMPLE);
    assert(input && "could not read expression fixture");
    const std::string source((std::istreambuf_iterator<char>(input)), {});
    SourceManager source_manager(source);
    Lexer lexer(source_manager);
    ParserDriver parser;
    assert(parser.parse(lexer));
    auto* program = dynamic_cast<Program*>(parser.get_ast());
    assert(program);
    SemanticAnalyzer analyzer;
    assert(analyzer.analyze(program));
    if (optimize) {
        AstOptimizer optimizer(analyzer.get_model());
        optimizer.optimize(program);
    }
    WasmCodegen codegen;
    const std::string wat = codegen.generate_wat(*program, analyzer.get_model());

    const auto run = [&](const char* name, std::vector<std::string> arguments = {}) {
        auto* routine = analyzer.get_global_scope().lookup_routine(name, false);
        assert(routine && routine->is_defined);
        auto result = execute_wasm(wat, *routine, arguments);
        if (!result.diagnostic.empty())
            std::cerr << name << " diagnostic: " << result.diagnostic << '\n';
        return result;
    };
    const auto int_result = [&](const char* name, std::int32_t expected,
                                std::vector<std::string> arguments = {}) {
        const auto result = run(name, std::move(arguments));
        assert(result.exit_code == 0);
        assert(std::get<std::int32_t>(result.value) == expected);
        return result.stdout_text;
    };
    const auto real_result = [&](const char* name, double expected) {
        const auto result = run(name);
        assert(result.exit_code == 0);
        assert(std::get<double>(result.value) == expected);
        return result.stdout_text;
    };

    // Arithmetic, operator precedence, and unary operators.
    int_result("arithmetic", 9);
    int_result("div_mod", -31);
    real_result("mixed", 3.5);
    int_result("unary", 0);

    // Comparisons and logic (and/or short-circuit).
    int_result("comparisons", 6);
    int_result("real_comparisons", 3);
    int_result("logic", 13);

    // real -> integer rounds half away from zero.
    int_result("round_pos", 3);
    int_result("round_neg", -3);
    int_result("round_275", 3);
    int_result("round_225", 2);

    // Boolean conversions: valid values convert, other values trap at runtime.
    assert(std::get<bool>(run("strict_bool", {"1"}).value));
    assert(!std::get<bool>(run("strict_bool", {"0"}).value));
    assert(run("strict_bool", {"2"}).exit_code != 0);
    assert(std::get<bool>(run("int_to_bool", {"1"}).value));
    assert(run("int_to_bool", {"2"}).exit_code != 0);
    int_result("bool_to_int", 1);
    real_result("bool_to_real", 1.0);

    // Equality across numeric and boolean operands.
    int_result("mixed_equality", 3);

    // Operands and arguments are evaluated left to right, exactly once.
    int_result("effects", 123007);
    int_result("argument_order", 123123);

    // Integer division wraps on the INT32_MIN / -1 edge case.
    int_result("divide", INT32_MIN, {"-2147483648", "-1"});
    int_result("divide", -3, {"-7", "2"});

    // Exact print formatting: spaces, newline, real fractional part, booleans.
    const auto printed = run("print_format");
    assert(printed.exit_code == 0);
    assert(printed.stdout_text == "42 true 2.5\n");
    const auto negative = run("print_negative");
    assert(negative.exit_code == 0);
    assert(negative.stdout_text == "-7 false -2.5\n");

    std::cout << "expression execution tests passed\n";
    return 0;
}
