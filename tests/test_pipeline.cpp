#include <iostream>
#include <fstream>
#include <cassert>
#include <filesystem>
#include <vector>
#include "TestUtils.hpp"
#include "SourceManager.hpp"
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "ASTPrinter.hpp"

void run_pipeline_test(const std::string& sample_name, bool to_file = false) {
    std::string path = resolve_path(sample_name);
    std::string code = read_file(path);
    assert(!code.empty() && "Sample not found or empty!");

    std::filesystem::path p(path);
    std::string stem = p.stem().string();

    std::ofstream out_file;
    std::string out_path;

    if (to_file) {
        out_path = (test_output_dir() / (stem + ".txt")).string();
        out_file.open(out_path, std::ios::out | std::ios::trunc);
        if (!out_file.is_open()) {
            std::cerr << "[ERROR] Could not open output file: " << out_path << "\n";
            return;
        }
    }

    std::ostream& out = to_file ? out_file : std::cout;

    SourceManager sm(code);
    Lexer lexer(sm);
    ParserDriver driver;

    if (!to_file) {
        std::cout << "Testing pipeline for: " << sample_name << "...\n";
    } else {
        std::cout << "Testing pipeline: " << sample_name << " -> " << out_path << "\n";
    }

    bool parse_success = driver.parse(lexer);

    if (!parse_success) {
        out << "=== Parsing Failed ===\n";
        for (const auto& err : driver.get_errors()) {
            out << "Error at " << err.location.row << ":" << err.location.column
                << " - " << err.message << "\n";
        }
    }

    assert(parse_success && "Parsing failed!");

    auto ast = driver.get_ast();
    assert(ast != nullptr && "AST is null!");

    AstPrinter printer(out);
    ast->accept(printer);
    out << "\n";

    if (!to_file) {
        std::cout << "[PASS] " << sample_name << "\n\n";
    }
}

void run_pipeline_error_test(const std::string& sample_name, const std::vector<size_t>& expected_rows) {
    std::string path = resolve_path(sample_name);
    std::string code = read_file(path);
    assert(!code.empty() && "Sample not found or empty!");

    std::string stem = std::filesystem::path(path).stem().string();
    std::string out_path = (test_output_dir() / (stem + ".txt")).string();
    std::ofstream out(out_path, std::ios::out | std::ios::trunc);
    assert(out.is_open() && "Cannot open output file!");

    std::cout << "Testing pipeline errors: " << sample_name << " -> " << out_path << "\n";

    SourceManager sm(code);
    Lexer lexer(sm);
    ParserDriver driver;
    bool parse_success = driver.parse(lexer);

    const auto& errors = driver.get_errors();
    out << "=== Parsing Failed (" << errors.size() << " errors) ===\n";
    for (const auto& err : errors) {
        out << "Error at " << err.location.row << ":" << err.location.column
            << " - " << err.message << "\n";
    }
    if (auto* ast = driver.get_ast()) {
        out << "\n=== Partial AST after recovery ===\n";
        AstPrinter printer(out);
        ast->accept(printer);
        out << "\n";
    }

    assert(!parse_success && "Parsing must fail!");
    assert(errors.size() == expected_rows.size() && "Unexpected number of errors!");
    for (size_t i = 0; i < errors.size(); ++i) {
        assert(errors[i].location.row == expected_rows[i] && "Error reported on unexpected line!");
    }
}

int main() {
    std::cout << "=== Running Pipeline Tests (AST Generation) ===\n";

    run_pipeline_test("01_print.imp", true);
    run_pipeline_test("02_variables.imp", true);
    run_pipeline_test("03_expressions.imp", true);
    run_pipeline_test("04_types_and_casting.imp", true);
    run_pipeline_test("05_logic_operators.imp", true);
    run_pipeline_test("06_scoping.imp", true);
    run_pipeline_test("07_while_loop.imp", true);
    run_pipeline_test("08_for_loop.imp", true);
    run_pipeline_test("09_routines.imp", true);
    run_pipeline_test("10_forward_declaration.imp", true);
    run_pipeline_test("11_binsearch.imp", true);
    run_pipeline_test("12_product_except_self.imp", true);
    run_pipeline_test("13_crossover.imp", true);
    run_pipeline_test("14_heap.imp", true);
    run_pipeline_test("15_graph_dfs.imp", true);
    run_pipeline_test("16_records.imp", true);
    run_pipeline_test("17_array_return.imp", true);
    run_pipeline_test("18_arrow_routines.imp", true);
    run_pipeline_test("19_record_return.imp", true);
    run_pipeline_test("20_array_of_array.imp", true);

    std::cout << "\n=== Running Pipeline Error Tests (Parser Recovery) ===\n";

    run_pipeline_error_test("21_syntax_error_single.imp", {5});
    run_pipeline_error_test("22_syntax_error_recovery.imp", {3, 5, 10});
    run_pipeline_error_test("23_syntax_error_declarations.imp", {3, 6, 10});
    run_pipeline_error_test("24_syntax_error_lexical.imp", {2, 3, 4});

    std::cout << "\n[ALL PASS] 20 AST outputs and 4 error reports generated in " << test_output_dir().string() << "\n";
    return 0;
}