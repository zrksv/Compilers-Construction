#include <iostream>
#include <cassert>
#include "TestUtils.hpp"
#include "SourceManager.hpp"
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "ASTPrinter.hpp"

void run_pipeline_test(const std::string& sample_name) {
    std::string path = resolve_path(sample_name);
    std::string code = read_file(path);

    SourceManager sm(code);
    Lexer lexer(sm);
    ParserDriver driver;

    std::cout << "Testing pipeline for: " << sample_name << "...\n";

    bool parse_success = driver.parse(lexer);
    assert(parse_success && "Parsing failed!");

    auto ast = driver.get_ast();
    assert(ast != nullptr && "AST is null!");

    AstPrinter printer(std::cout);
    ast->accept(printer);

    std::cout << "\n[PASS] " << sample_name << "\n\n";
}

int main() {
    run_pipeline_test("01_print.imp");
    run_pipeline_test("02_variables.imp");
    run_pipeline_test("03_expressions.imp");
    run_pipeline_test("04_types_and_casting.imp");
    run_pipeline_test("05_logic_operators.imp");
    run_pipeline_test("06_scoping.imp");
    run_pipeline_test("07_while_loop.imp");
    run_pipeline_test("08_for_loop.imp");
    run_pipeline_test("09_routines.imp");
    run_pipeline_test("10_forward_declaration.imp");
    run_pipeline_test("10_error_no_forward.imp");
    run_pipeline_test("11_binsearch.imp");
    run_pipeline_test("12_product_except_self.imp");
    run_pipeline_test("13_crossover.imp");
    run_pipeline_test("14_heap.imp");
    run_pipeline_test("15_graph_dfs.imp");
    run_pipeline_test("16_records.imp");
    run_pipeline_test("17_array_return.imp");
    run_pipeline_test("18_arrow_routines.imp");
    run_pipeline_test("19_record_return.imp");
    run_pipeline_test("20_array_of_array.imp");
    std::cout << "Pipeline tests ready.\n";
    return 0;
}