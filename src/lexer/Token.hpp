#pragma once
#include "../common//Location.hpp"
#include <string>

enum class TokenType {
    Identifier,
    Integer,
    Real,
    Bool,
    IntegerLiteral,
    RealLiteral,
    True,
    False,

    Var,
    Type,
    Routine,
    Is,
    End,

    While,
    For,
    Loop,
    Reverse,
    In,
    If,
    Then,
    Else,

    Array,
    Record,

    And,
    Or,
    Xor,
    Not,

    Print,
    Return,
    Size,

    Plus,
    Minus,
    Mul,
    Div,
    Mod,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Equal,
    NotEqual,
    Assign,
    Range,
    Arrow,

    LeftPar,
    RightPar,
    LeftBr,
    RightBr,
    Colon,
    Semicolon,
    Comma,
    Dot,

    Eof,
    NewLine,
    Unknown
};

struct Token {
    TokenType type;
    std::string text;
    Location location;
};

std::string token_to_string(TokenType type);
