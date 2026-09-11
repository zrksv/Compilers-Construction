#include "Lexer.hpp"
#include <cctype>

void Lexer::skip_whitespaces() {
    char c = source.peek();
    while (!source.is_eof() && (c == ' ' || c == '\t' || c == '\r')) {
        source.get();
        c = source.peek();
    }
}

Token Lexer::lex_word() {
    Location loc = source.get_location();
    char c = source.get();
    return Token{TokenType::Unknown, std::string(1, c), loc};
}

Token Lexer::lex_number() {
    Location loc = source.get_location();
    char c = source.get();
    return Token{TokenType::Unknown, std::string(1, c), loc};
}

Token Lexer::lex_oper() {
    Location loc = source.get_location();
    char c = source.get();
    return Token{TokenType::Unknown, std::string(1, c), loc};
}

Token Lexer::next_token() {
    skip_whitespaces();
    Location location = source.get_location();

    if (source.is_eof()) {
        return Token{TokenType::Eof, "", location};
    }

    char c = source.peek();

    if (c == '\n') {
        source.get();
        return Token{TokenType::NewLine, "\n", location};
    }
    if (std::isalpha(c) || c == '_') {
        return lex_word();
    }
    if (std::isdigit(c)) {
        return lex_number();
    }
    return lex_oper();
}
