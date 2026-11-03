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
#include "SemanticAnalyzer.hpp"
#include "optimizer/AstOptimizer.hpp"

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

    SemanticAnalyzer analyzer;
    bool semantic_success = analyzer.analyze(dynamic_cast<Program*>(ast));
    if (!semantic_success) {
        out << "=== Semantic Analysis Failed ===\n";
        analyzer.get_diagnostics().print(out);
        analyzer.get_diagnostics().print(std::cerr);
    }
    assert(semantic_success && "Semantic analysis failed!");
    out << "=== Semantic Analysis OK ===\n";

    auto* program = dynamic_cast<Program*>(ast);
    AstOptimizer optimizer(analyzer.get_model());
    if (program && optimizer.optimize(program)) {
        out << "\n=== Optimized AST ===\n";
        program->accept(printer);
        out << "\n";

        SemanticAnalyzer reanalyzer;
        bool still_valid = reanalyzer.analyze(program);
        if (!still_valid) {
            reanalyzer.get_diagnostics().print(std::cerr);
        }
        assert(still_valid && "Optimized AST must stay semantically valid");
    }

    if (!to_file) {
        std::cout << "[PASS] " << sample_name << "\n\n";
    }
}

void run_pipeline_semantic_error_test(const std::string& sample_name, const std::vector<size_t>& expected_rows) {
    std::string path = resolve_path(sample_name);
    std::string code = read_file(path);
    assert(!code.empty() && "Sample not found or empty!");

    std::string stem = std::filesystem::path(path).stem().string();
    std::string out_path = (test_output_dir() / (stem + ".txt")).string();
    std::ofstream out(out_path, std::ios::out | std::ios::trunc);
    assert(out.is_open() && "Cannot open output file!");

    std::cout << "Testing pipeline semantic errors: " << sample_name << " -> " << out_path << "\n";

    SourceManager sm(code);
    Lexer lexer(sm);
    ParserDriver driver;
    bool parse_success = driver.parse(lexer);
    assert(parse_success && "Parsing failed!");

    out << "=== AST ===\n";
    AstPrinter printer(out);
    driver.get_ast()->accept(printer);
    out << "\n";

    SemanticAnalyzer analyzer;
    bool semantic_success = analyzer.analyze(dynamic_cast<Program*>(driver.get_ast()));

    const auto& errors = analyzer.get_diagnostics().get_errors();
    out << "=== Semantic Analysis Failed (" << errors.size() << " errors) ===\n";
    analyzer.get_diagnostics().print(out);

    assert(!semantic_success && "Semantic analysis must fail!");
    assert(errors.size() == expected_rows.size() && "Unexpected number of semantic errors!");
    for (size_t i = 0; i < errors.size(); ++i) {
        assert(errors[i].location.row == expected_rows[i] && "Semantic error reported on unexpected line!");
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
    std::cout << "=== Running Pipeline Tests (AST Generation and Semantic Analysis) ===\n";

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
    run_pipeline_test("28_optimization_demo.imp", true);

    std::cout << "\n=== Running Pipeline Semantic Error Tests ===\n";

    run_pipeline_semantic_error_test("10_error_no_forward.imp", {4});
    run_pipeline_semantic_error_test("25_semantic_types.imp", {16, 19, 20});
    run_pipeline_semantic_error_test("26_semantic_routines.imp", {6, 9, 16, 18});
    run_pipeline_semantic_error_test("27_semantic_scopes_readonly.imp", {14, 16, 18, 19});
    run_pipeline_semantic_error_test("29_semantic_checks.imp", {3, 4, 5});

    std::cout << "\n=== Running Pipeline Error Tests (Parser Recovery) ===\n";

    run_pipeline_error_test("21_syntax_error_single.imp", {5});
    run_pipeline_error_test("22_syntax_error_recovery.imp", {3, 5, 10});
    run_pipeline_error_test("23_syntax_error_declarations.imp", {3, 6, 10});
    run_pipeline_error_test("24_syntax_error_lexical.imp", {2, 3, 4});

    std::cout << "\n[ALL PASS] 21 AST outputs, 5 semantic and 4 syntax error reports generated in " << test_output_dir().string() << "\n";
    return 0;
}