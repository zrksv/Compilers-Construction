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
    std::string text = "";
    bool is_real = false;

    while (!source.is_eof() && std::isdigit(source.peek())) {
        text += source.get();
    }

    if (!source.is_eof() && source.peek() == '.') {
        if (source.peek_next() != '.') {
            is_real = true;
            text += source.get();

            while (!source.is_eof() && std::isdigit(source.peek())) {
                text += source.get();
            }
        }
    }

    TokenType type = is_real ? TokenType::RealLiteral : TokenType::IntegerLiteral;
    return Token{type, text, loc};
}

Token Lexer::lex_oper() {
    Location loc = source.get_location();
    char c = source.get();
    std::string text(1, c);

    switch (c) {
        case '+': return Token{TokenType::Plus, text, loc};
        case '-': return Token{TokenType::Minus, text, loc};
        case '*': return Token{TokenType::Mul, text, loc};
        case '%': return Token{TokenType::Rem, text, loc};
        case '(': return Token{TokenType::LeftPar, text, loc};
        case ')': return Token{TokenType::RightPar, text, loc};
        case '[': return Token{TokenType::LeftBr, text, loc};
        case ']': return Token{TokenType::RightBr, text, loc};
        case ',': return Token{TokenType::Comma, text, loc};
        case ';': return Token{TokenType::Semicolon, text, loc};

        case ':':
            if (source.peek() == '=') {
                text += source.get();
                return Token{TokenType::Assign, text, loc};
            }
            return Token{TokenType::Colon, text, loc};

        case '/':
            if (source.peek() == '=') {
                text += source.get();
                return Token{TokenType::NotEqual, text, loc};
            }
            return Token{TokenType::Div, text, loc};

        case '<':
            if (source.peek() == '=') {
                text += source.get();
                return Token{TokenType::LessEqual, text, loc};
            }
            return Token{TokenType::Less, text, loc};

        case '>':
            if (source.peek() == '=') {
                text += source.get();
                return Token{TokenType::GreaterEqual, text, loc};
            }
            return Token{TokenType::Greater, text, loc};

        case '=':
            if (source.peek() == '>') {
                text += source.get();
                return Token{TokenType::Arrow, text, loc};
            }
            return Token{TokenType::Equal, text, loc};

        case '.':
            if (source.peek() == '.') {
                text += source.get();
                return Token{TokenType::Range, text, loc};
            }
            return Token{TokenType::Dot, text, loc};

        default:
            return Token{TokenType::Unknown, text, loc};
    }
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
