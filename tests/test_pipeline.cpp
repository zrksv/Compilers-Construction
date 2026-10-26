#include <iostream>
#include <fstream>
#include <cassert>
#include <filesystem>
#include "TestUtils.hpp"
#include "SourceManager.hpp"
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "ASTPrinter.hpp"

void run_pipeline_test(const std::string& sample_name, bool to_file = false) {
    std::string path = resolve_path(sample_name);
    std::string code = read_file(path);

    std::filesystem::path p(path);
    std::string stem = p.stem().string();

    std::ofstream out_file;
    std::string out_path;

    if (to_file) {
        std::filesystem::path out_dir = get_build_output_dir("pipeline");
        out_path = (out_dir / (stem + ".txt")).string();
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

    std::cout << "\n[ALL PASS] All 20 AST outputs generated in build/tests/output/pipeline/\n";
    return 0;
}