#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "SourceManager.hpp"
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "ASTPrinter.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: fair_compiler <source.imp> [--tokens] [routine_name] [args...]\n";
        return 1;
    }

    string filename = argv[1];
    bool print_tokens = false;

    if (argc >= 3 && string(argv[2]) == "--tokens") {
        print_tokens = true;
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

    if (print_tokens) {
        cout << "--- Tokens ---\n";
        Token tok;
        do {
            tok = lexer.next_token();
            cout << left << setw(8) << (to_string(tok.location.row) + ":" + to_string(tok.location.column))
                 << " " << setw(18) << token_to_string(tok.type)
                 << " '" << tok.text << "'\n";
        } while (tok.type != TokenType::Eof);
        return 0;
    }

    ParserDriver driver;
    bool parse_success = driver.parse(lexer);

    if (parse_success) {
        cout << "Parsing successful! AST:\n";
        auto ast = driver.get_ast();
        if (ast) {
            AstPrinter printer(cout);
            ast->accept(printer);
            cout << "\n";
        }
    } else {
        cerr << "Parsing failed due to syntax errors:\n";
        for (const auto& err : driver.get_errors()) {
            cerr << "Error at " << err.location.row << ":" << err.location.column
                 << " - " << err.message << "\n";
        }
        return 1;
    }

    return 0;
}