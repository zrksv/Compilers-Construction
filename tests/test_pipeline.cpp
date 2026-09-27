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
    std::cout << "Pipeline tests ready.\n";
    return 0;
}