#ifndef LEXER_HPP
#define LEXER_HPP
#include "SourceManager.hpp"
#include "Token.hpp"

class Lexer {
private:
    SourceManager& source;

    void skip_whitespaces();
    Token lex_word();
    Token lex_number();
    Token lex_oper();

public:
    explicit Lexer(SourceManager& src) : source(src) {}

    Token next_token();
};

#endif //LEXER_HPP
