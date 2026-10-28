#pragma once
#include "../common/SourceManager.hpp"
#include "Token.hpp"

class Lexer {
private:
    SourceManager& source;

    void skip_whitespaces();
    bool skip_line_continuation();
    Token lex_word();
    Token lex_number();
    Token lex_oper();

public:
    explicit Lexer(SourceManager& src) : source(src) {}

    Token next_token();
};
