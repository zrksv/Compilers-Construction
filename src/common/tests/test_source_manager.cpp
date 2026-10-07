#include <iostream>
#include <cassert>
#include <vector>
#include "../src/common/SourceManager.hpp"
#include "../../lexer/Token.hpp"
#include "../../lexer/Lexer.hpp"

std::vector<TokenType> tokenize_types(const std::string& code) {
    SourceManager sm(code);
    Lexer lexer(sm);
    std::vector<TokenType> types;
    while (true) {
        Token tok = lexer.next_token();
        types.push_back(tok.type);
        if (tok.type == TokenType::Eof) break;
    }
    return types;
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

int main() {
    test_source_manager();
    test_newlines();
}