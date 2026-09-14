#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "SourceManager.hpp"
#include "Lexer.hpp"

using namespace std;

static string escape_token_text(const string& text) {
    string res;
    for (char c : text) {
        if (c == '\n') res += "\\n";
        else if (c == '\t') res += "\\t";
        else if (c == '\r') res += "\\r";
        else if (c == '\\') res += "\\\\";
        else res += c;
    }
    return res;
}

int main(int argc, char* argv[]) {
    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << argv[1] << "\n";
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();

    SourceManager sm(buffer.str());
    Lexer lexer(sm);

    int count = 0;
    int error_count = 0;

    cout << "--- Tokens ---\n";
    while (true) {
        Token tok = lexer.next_token();
        count++;

        if (tok.type == TokenType::Unknown) {
            error_count++;
        }

        string loc = to_string(tok.location.row) + ":" + to_string(tok.location.column);
        cout << left << setw(8) << loc
             << " " << setw(18) << token_to_string(tok.type)
             << " '" << escape_token_text(tok.text) << "'\n";

        if (tok.type == TokenType::Eof) {
            break;
        }
    }

    cout << "----------------------------------------\n";
    cout << "Total: " << count << " token(s) scanned";
    if (error_count > 0) {
        cout << ", " << error_count << " error(s) found";
    }
    cout << "\n";

    return (error_count == 0) ? 0 : 1;
}