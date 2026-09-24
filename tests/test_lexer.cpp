#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <filesystem>
#include "common/SourceManager.hpp"
#include "lexer/Lexer.hpp"

std::string escape_token_text(const std::string& text) {
    std::string res;
    for (char c : text) {
        if (c == '\n') res += "\\n";
        else if (c == '\t') res += "\\t";
        else if (c == '\r') res += "\\r";
        else if (c == '\\') res += "\\\\";
        else res += c;
    }
    return res;
}

void report_unknown_token(const std::string& code,
                          const Token& tok,
                          const std::string& filename = "<memory>",
                          std::ostream& out = std::cout) {
    out << "\n[LEXICAL ERROR] Unexpected token '" 
        << escape_token_text(tok.text) << "' at " 
        << filename << ":" << tok.location.row << ":" << tok.location.column 
        << "\n";

    std::istringstream stream(code);
    std::string line_text;
    size_t current_row = 1;

    while (current_row <= tok.location.row && std::getline(stream, line_text)) {
        if (current_row == tok.location.row) {
            break;
        }
        current_row++;
    }

    if (current_row == tok.location.row) {
        std::string row_str = std::to_string(tok.location.row);

        out << row_str << " | " << line_text << "\n";
        out << std::string(row_str.length(), ' ') << " | ";

        for (size_t col = 1; col < tok.location.column && col <= line_text.length(); ++col) {
            if (line_text[col - 1] == '\t') {
                out << '\t';
            } else {
                out << ' ';
            }
        }
        out << "^--- unknown token\n";
    }
}

std::vector<TokenType> tokenize_types(const std::string& code) {
    SourceManager sm(code);
    Lexer lexer(sm);
    std::vector<TokenType> types;

    while (true) {
        Token tok = lexer.next_token();
        types.push_back(tok.type);
        if (tok.type == TokenType::Eof) {
            break;
        }
    }
    return types;
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static std::filesystem::path find_tests_dir() {
    std::filesystem::path cur = std::filesystem::current_path();
    for (int i = 0; i < 5; ++i) {
        if (std::filesystem::exists(cur / "tests" / "samples")) {
            return cur / "tests";
        }
        if (std::filesystem::exists(cur / "samples")) {
            return cur;
        }
        if (!cur.has_parent_path()) break;
        cur = cur.parent_path();
    }
    return "tests";
}

std::string resolve_path(const std::string& name) {
    std::string file = name.ends_with(".imp") ? name : name + ".imp";
    return (find_tests_dir() / "samples" / file).string();
}

void run_sample(const std::string& sample_name, bool to_file = false) {
    std::string path = resolve_path(sample_name);
    if (path.empty()) {
        std::cerr << "[ERROR] Could not find file: " << sample_name << "\n";
        return;
    }

    std::string code = read_file(path);
    if (code.empty()) {
        std::cerr << "[ERROR] File is empty or could not be read: " << path << "\n";
        return;
    }

    std::filesystem::path p(path);
    std::string stem = p.stem().string();

    std::ofstream out_file;
    std::string out_path;

    if (to_file) {
        std::filesystem::path out_dir = find_tests_dir() / "output";
        std::filesystem::create_directories(out_dir);
        out_path = (out_dir / (stem + ".txt")).string();
        out_file.open(out_path, std::ios::out | std::ios::trunc);
        if (!out_file.is_open()) {
            std::cerr << "[ERROR] Could not open output file: " << out_path << "\n";
            return;
        }
    }

    std::ostream& out = to_file ? out_file : std::cout;

    if (!to_file) {
        std::cout << "=== Running: " << path << " ===\n";
    } else {
        out << "=== Tokens: " << path << " ===\n";
    }

    SourceManager sm(code);
    Lexer lexer(sm);
    size_t count = 0;
    std::vector<Token> unknown_tokens;

    while (true) {
        Token tok = lexer.next_token();
        count++;

        out << "Line " << std::setw(2) << std::right << tok.location.row 
            << ", col " << std::setw(2) << std::right << tok.location.column 
            << " | " << std::setw(15) << std::left << token_to_string(tok.type) 
            << " ('" << escape_token_text(tok.text) << "')\n";

        if (tok.type == TokenType::Unknown) {
            unknown_tokens.push_back(tok);
            if (!to_file) {
                report_unknown_token(code, tok, path, std::cout);
            }
        }

        if (tok.type == TokenType::Eof) break;
    }

    out << "\n--- Summary: " << count << " tokens, " << unknown_tokens.size() << " unknown ---\n";

    if (to_file) {
        if (!unknown_tokens.empty()) {
            out << "\n=== Lexical Errors (" << unknown_tokens.size() << ") ===\n";
            size_t max_errors_to_print = std::min(unknown_tokens.size(), size_t(20));
            for (size_t i = 0; i < max_errors_to_print; ++i) {
                report_unknown_token(code, unknown_tokens[i], path, out);
            }
            if (unknown_tokens.size() > max_errors_to_print) {
                out << "\n... and " << (unknown_tokens.size() - max_errors_to_print) << " more unknown tokens.\n";
            }
        }

        std::cout << "=== Running: " << path << " ===\n";
        std::cout << "Tokens written to: " << out_path << "\n";
        std::cout << "--- Summary: " << count << " tokens, " << unknown_tokens.size() << " unknown ---\n\n";
    } else {
        std::cout << "\n";
    }
}

void test_source_manager() {
    SourceManager sm("ab\n c\t\n");
    assert(sm.peek() == 'a');
    assert(sm.peek_next() == 'b');
    assert(sm.get() == 'a');
    assert(sm.get_location().column == 2);

    assert(sm.get() == 'b');
    assert(sm.get() == '\n');
    assert(sm.get_location().row == 2);
    assert(sm.get_location().column == 1);

    assert(sm.get() == ' ');
    assert(sm.get() == 'c');
    assert(sm.get() == '\t');
    assert(sm.get() == '\n');
    assert(sm.get_location().row == 3);
    assert(sm.get_location().column == 1);
    assert(sm.is_eof());
    std::cout << "[PASS] test_source_manager\n";
}

void test_newlines() {
    auto types = tokenize_types("  \t\n  \n");
    assert(types.size() == 3);
    assert(types[0] == TokenType::NewLine);
    assert(types[1] == TokenType::NewLine);
    assert(types[2] == TokenType::Eof);
    std::cout << "[PASS] test_newlines\n";
}

void test_keywords() {
    std::string code = "var variable routine routines is end return size if format while loop for reverse in then else print and or xor not true false type array record integer real boolean bool VAR Routine _ident my_var_123";
    auto types = tokenize_types(code);

    assert(types[0] == TokenType::Var);
    assert(types[1] == TokenType::Identifier);
    assert(types[2] == TokenType::Routine);
    assert(types[3] == TokenType::Identifier);
    assert(types[4] == TokenType::Is);
    assert(types[5] == TokenType::End);
    assert(types[6] == TokenType::Return);
    assert(types[7] == TokenType::Size);
    assert(types[8] == TokenType::If);
    assert(types[9] == TokenType::Identifier);
    assert(types[10] == TokenType::While);
    assert(types[11] == TokenType::Loop);
    assert(types[12] == TokenType::For);
    assert(types[13] == TokenType::Reverse);
    assert(types[14] == TokenType::In);
    assert(types[15] == TokenType::Then);
    assert(types[16] == TokenType::Else);
    assert(types[17] == TokenType::Print);
    assert(types[18] == TokenType::And);
    assert(types[19] == TokenType::Or);
    assert(types[20] == TokenType::Xor);
    assert(types[21] == TokenType::Not);
    assert(types[22] == TokenType::True);
    assert(types[23] == TokenType::False);
    assert(types[24] == TokenType::Type);
    assert(types[25] == TokenType::Array);
    assert(types[26] == TokenType::Record);
    assert(types[27] == TokenType::Integer);
    assert(types[28] == TokenType::Real);
    assert(types[29] == TokenType::Bool);
    assert(types[30] == TokenType::Identifier);
    assert(types[31] == TokenType::Identifier);
    assert(types[32] == TokenType::Identifier);
    assert(types[33] == TokenType::Identifier);
    assert(types[34] == TokenType::Identifier);
    assert(types[35] == TokenType::Eof);
    std::cout << "[PASS] test_keywords\n";
}

void test_numbers() {
    std::string code = "0 42 12345 3.14 0.05 1..10";
    auto types = tokenize_types(code);

    assert(types[0] == TokenType::IntegerLiteral);
    assert(types[1] == TokenType::IntegerLiteral);
    assert(types[2] == TokenType::IntegerLiteral);
    assert(types[3] == TokenType::RealLiteral);
    assert(types[4] == TokenType::RealLiteral);
    assert(types[5] == TokenType::IntegerLiteral);
    assert(types[6] == TokenType::Range);
    assert(types[7] == TokenType::IntegerLiteral);
    std::cout << "[PASS] test_numbers\n";
}

void test_operators() {
    std::string code = ":= : /= / <= < >= > => = .. . + - * % ( ) [ ] , ;";
    auto types = tokenize_types(code);

    assert(types[0] == TokenType::Assign);
    assert(types[1] == TokenType::Colon);
    assert(types[2] == TokenType::NotEqual);
    assert(types[3] == TokenType::Div);
    assert(types[4] == TokenType::LessEqual);
    assert(types[5] == TokenType::Less);
    assert(types[6] == TokenType::GreaterEqual);
    assert(types[7] == TokenType::Greater);
    assert(types[8] == TokenType::Arrow);
    assert(types[9] == TokenType::Equal);
    assert(types[10] == TokenType::Range);
    assert(types[11] == TokenType::Dot);
    assert(types[12] == TokenType::Plus);
    assert(types[13] == TokenType::Minus);
    assert(types[14] == TokenType::Mul);
    assert(types[15] == TokenType::Mod);
    assert(types[16] == TokenType::LeftPar);
    assert(types[17] == TokenType::RightPar);
    assert(types[18] == TokenType::LeftBr);
    assert(types[19] == TokenType::RightBr);
    assert(types[20] == TokenType::Comma);
    assert(types[21] == TokenType::Semicolon);
    std::cout << "[PASS] test_operators\n";
}

void test_errors() {
    std::string code = "var x is @;\nprint $;\n";
    SourceManager sm(code);
    Lexer lexer(sm);

    bool found_at = false;
    bool found_dollar = false;

    while (true) {
        Token tok = lexer.next_token();

        if (tok.text == "@") {
            assert(tok.type == TokenType::Unknown);
            assert(tok.location.row == 1);
            assert(tok.location.column == 10);
            report_unknown_token(code, tok, "<memory>");
            found_at = true;
        } else if (tok.text == "$") {
            assert(tok.type == TokenType::Unknown);
            assert(tok.location.row == 2);
            assert(tok.location.column == 7);
            report_unknown_token(code, tok, "<memory>");
            found_dollar = true;
        }

        if (tok.type == TokenType::Eof) {
            break;
        }
    }

    assert(found_at && found_dollar);
    std::cout << "\n[PASS] test_errors\n\n";
}

int main() {
    test_source_manager();
    test_newlines();
    test_errors();
    test_keywords();
    test_numbers();
    test_operators();

    run_sample("01_print.imp", true);
    run_sample("02_variables.imp", true);
    run_sample("03_expressions.imp", true);
    run_sample("04_types_and_casting.imp", true);
    run_sample("05_logic_operators.imp", true);
    run_sample("06_scoping.imp", true);
    run_sample("07_while_loop.imp", true);
    run_sample("08_for_loop.imp", true);
    run_sample("09_routines.imp", true);
    run_sample("10_forward_declaration.imp", true);
    run_sample("10_error_no_forward.imp", true);
    run_sample("11_binsearch.imp", true);
    run_sample("12_product_except_self.imp", true);
    run_sample("13_crossover.imp", true);
    run_sample("14_heap.imp", true);
    run_sample("15_graph_dfs.imp", true);
    run_sample("16_records.imp", true);
    run_sample("17_array_return.imp", true);
    run_sample("18_arrow_routines.imp", true);
    run_sample("19_record_return.imp", true);
    run_sample("20_array_of_array.imp", true);

    return 0;
}
