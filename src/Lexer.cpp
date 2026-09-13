#include "Lexer.hpp"
#include <cctype>
#include <unordered_map>

void Lexer::skip_whitespaces() {
    char c = source.peek();
    while (!source.is_eof() && (c == ' ' || c == '\t' || c == '\r')) {
        source.get();
        c = source.peek();
    }
}

Token Lexer::lex_word() {
    Location loc = source.get_location();
    std::string text;

    while (!source.is_eof()) {
        char c = source.peek();
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
            text += source.get();
        } else {
            break;
        }
    }

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"var", TokenType::Var},
        {"type", TokenType::Type},
        {"routine", TokenType::Routine},
        {"is", TokenType::Is},
        {"end", TokenType::End},
        {"integer", TokenType::Integer},
        {"real", TokenType::Real},
        {"boolean", TokenType::Bool},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"array", TokenType::Array},
        {"record", TokenType::Record},
        {"while", TokenType::While},
        {"for", TokenType::For},
        {"loop", TokenType::Loop},
        {"reverse", TokenType::Reverse},
        {"in", TokenType::In},
        {"if", TokenType::If},
        {"then", TokenType::Then},
        {"else", TokenType::Else},
        {"and", TokenType::And},
        {"or", TokenType::Or},
        {"xor", TokenType::Xor},
        {"not", TokenType::Not},
        {"print", TokenType::Print},
        {"return", TokenType::Return},
        {"size", TokenType::Size},
    };

    auto it = keywords.find(text);
    if (it != keywords.end()) {
        return Token{it->second, text, loc};
    }

    return Token{TokenType::Identifier, text, loc};
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
