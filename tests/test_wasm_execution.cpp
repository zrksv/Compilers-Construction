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

#ifndef IMP_WASM_SAMPLE
#error IMP_WASM_SAMPLE must name the end-to-end fixture
#endif

int main(int argc, char** argv) {
    const bool optimize = argc < 2 || std::string(argv[1]) != "--no-opt";
    std::ifstream input(IMP_WASM_SAMPLE);
    assert(input && "could not read WASM integration fixture");
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
    assert(codegen.generate_wat(*program, analyzer.get_model()) == wat);

    SourceManager empty_source_manager("");
    Lexer empty_lexer(empty_source_manager);
    ParserDriver empty_parser;
    assert(empty_parser.parse(empty_lexer));
    SemanticAnalyzer empty_analyzer;
    auto* empty_program = dynamic_cast<Program*>(empty_parser.get_ast());
    assert(empty_program && empty_analyzer.analyze(empty_program));
    WasmCodegen empty_codegen;
    const auto empty_wat = empty_codegen.generate_wat(*empty_program, empty_analyzer.get_model());
    assert(!empty_wat.empty());
    RoutineSymbol missing_routine("missing_entry", {}, PrimitiveType::get_integer());
    const auto empty_execution = execute_wasm(empty_wat, missing_routine, {});
    assert(empty_execution.exit_code != 0 && !empty_execution.diagnostic.empty());

    const auto run = [&](const char* name, std::vector<std::string> args = {}) {
        auto* routine = analyzer.get_global_scope().lookup_routine(name, false);
        assert(routine && routine->is_defined);
        auto result = execute_wasm(wat, *routine, args);
        if (!result.diagnostic.empty())
            std::cerr << name << " diagnostic: " << result.diagnostic << '\n';
        return result;
    };
    const auto int_result = [&](const char* name, std::int32_t expected, std::vector<std::string> args = {}) {
        const auto result = run(name, std::move(args));
        assert(result.exit_code == 0);
        assert(std::get<std::int32_t>(result.value) == expected);
        return result.stdout_text;
    };

    auto add = run("add", {"20", "22"});
    assert(add.exit_code == 0 && std::get<std::int32_t>(add.value) == 42);
    assert(add.stdout_text.empty() && add.diagnostic.empty());
    assert(std::get<bool>(run("even", {"8"}).value));
    assert(!std::get<bool>(run("even", {"7"}).value));
    assert(int_result("sum", 15, {"5"}) == "");
    assert(int_result("control", 3) == "");
    assert(int_result("ranges", 12) == "");
    assert(int_result("evaluated_bounds", 1221) == "");
    assert(int_result("range_max", 1, {"2147483647"}) == "");
    assert(int_result("range_min_reverse", 1, {"-2147483648"}) == "");
    const auto reverse_pool = run("reverse_real_pool");
    assert(reverse_pool.exit_code == 0);
    assert(std::get<double>(reverse_pool.value) == 141.0);
    assert(int_result("nested_matrix", 77) == "");
    assert(int_result("array_sum", 6) == "");
    assert(int_result("init_global", 17) == "");
    assert(int_result("aggregate_default", 0) == "");
    assert(int_result("aggregate_reference", 23) == "");
    assert(int_result("aggregate_rebinding", 8) == "");
    assert(int_result("array_record_iteration", 7) == "");
    assert(int_result("short_circuit", 0) == "");
    assert(std::get<std::int32_t>(run("dynamic_short_circuit", {"false", "true"}).value) == 1);
    assert(std::get<std::int32_t>(run("dynamic_short_circuit", {"true", "true"}).value) == 3);
    const auto bad_index = run("bad_index", {"0"});
    assert(bad_index.exit_code != 0);
    assert(bad_index.diagnostic.find("231:18: array index out of bounds") != std::string::npos);
    const auto partial_trap = run("print_then_bad_index", {"0"});
    assert(partial_trap.exit_code != 0 && partial_trap.stdout_text == "99\n");
    assert(partial_trap.diagnostic.find("243:18: array index out of bounds") != std::string::npos);
    assert(run("strict_bool", {"2"}).exit_code != 0);
    assert(std::get<bool>(run("strict_bool", {"1"}).value));
    assert(std::get<double>(run("accept_real", {"42"}).value) == 42.0);
    assert(std::get<std::int32_t>(run("divide", {"-2147483648", "-1"}).value) == INT32_MIN);
    const auto reverse = run("reverse_print");
    assert(reverse.exit_code == 0 && reverse.stdout_text == "3\n2\n1\n");
    const auto printed = run("print_values");
    assert(printed.exit_code == 0);
    assert(printed.stdout_text == "42 true 2.5\n");
}
