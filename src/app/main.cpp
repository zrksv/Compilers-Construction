#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "SourceManager.hpp"
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "ASTPrinter.hpp"
#include "SemanticAnalyzer.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Error: routine name is not specified.\n";
        cerr << "Usage: fair_compiler <source.imp> <routine_name> [args...]\n";
        return 1;
    }

    string filename = argv[1];
    string routine_name = argv[2];
    vector<string> routine_args;
    for (int i = 3; i < argc; ++i) {
        routine_args.push_back(argv[i]);
    }

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << "\n";
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();

    SourceManager sm(buffer.str());
    Lexer lexer(sm);

    ParserDriver driver;
    bool parse_success = driver.parse(lexer);

    if (!parse_success) {
        cerr << "Parsing failed due to syntax errors:\n";
        for (const auto& err : driver.get_errors()) {
            cerr << "Error at " << err.location.row << ":" << err.location.column
                 << " - " << err.message << "\n";
        }
        return 1;
    }

    auto* program = dynamic_cast<Program*>(driver.get_ast());

    SemanticAnalyzer semantic_analyzer;
    if (!semantic_analyzer.analyze(program)) {
        cerr << "Semantic analysis failed:\n";
        semantic_analyzer.get_diagnostics().print(cerr);
        return 1;
    }

    // Check if the requested routine exists in the program
    RoutineDeclaration* target_routine = nullptr;

    if (program) {
        for (const auto& decl : program->declarations) {
            if (auto* r = dynamic_cast<RoutineDeclaration*>(decl.get())) {
                if (r->name == routine_name) {
                    target_routine = r;
                    break;
                }
            }
        }
    }

    if (!target_routine) {
        cerr << "Error: routine '" << routine_name << "' not found in '" << filename << "'.\n";
        return 1;
    }

    if (target_routine->body_type == RoutineBodyType::Forward) {
        cerr << "Error: routine '" << routine_name << "' is only forward-declared without a body.\n";
        return 1;
    }

    if (routine_args.size() != target_routine->parameters.size()) {
        cerr << "Error: routine '" << routine_name << "' expects "
             << target_routine->parameters.size() << " argument(s), but "
             << routine_args.size() << " provided.\n";
        return 1;
    }

    cout << "Parsing and semantic analysis successful! Routine '" << routine_name << "' verified for execution.\n";
    cout << "AST:\n";
    auto ast = driver.get_ast();
    if (ast) {
        AstPrinter printer(cout);
        ast->accept(printer);
        cout << "\n";
    }

    return 0;
}
