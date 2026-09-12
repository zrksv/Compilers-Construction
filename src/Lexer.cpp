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
