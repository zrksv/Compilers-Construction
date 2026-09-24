#include "Token.hpp"

std::string token_to_string(TokenType type) {
    switch (type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::Integer: return "Integer";
        case TokenType::Real: return "Real";
        case TokenType::Bool: return "Bool";
        case TokenType::IntegerLiteral: return "IntegerLiteral";
        case TokenType::RealLiteral: return "RealLiteral";
        case TokenType::True: return "True";
        case TokenType::False: return "False";

        case TokenType::Var: return "Var";
        case TokenType::Type: return "Type";
        case TokenType::Routine: return "Routine";
        case TokenType::Is: return "Is";
        case TokenType::End: return "End";

        case TokenType::While: return "While";
        case TokenType::For: return "For";
        case TokenType::Loop: return "Loop";
        case TokenType::Reverse: return "Reverse";
        case TokenType::In: return "In";
        case TokenType::If: return "If";
        case TokenType::Then: return "Then";
        case TokenType::Else: return "Else";

        case TokenType::Array: return "Array";
        case TokenType::Record: return "Record";

        case TokenType::And: return "And";
        case TokenType::Or: return "Or";
        case TokenType::Xor: return "Xor";
        case TokenType::Not: return "Not";

        case TokenType::Print: return "Print";
        case TokenType::Return: return "Return";
        case TokenType::Size: return "Size";

        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Mul: return "Mul";
        case TokenType::Div: return "Div";
        case TokenType::Mod: return "Mod";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::Equal: return "Equal";
        case TokenType::NotEqual: return "NotEqual";
        case TokenType::Assign: return "Assign";
        case TokenType::Range: return "Range";
        case TokenType::Arrow: return "Arrow";

        case TokenType::LeftPar: return "LeftPar";
        case TokenType::RightPar: return "RightPar";
        case TokenType::LeftBr: return "LeftBr";
        case TokenType::RightBr: return "RightBr";
        case TokenType::Colon: return "Colon";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";

        case TokenType::Eof: return "Eof";
        case TokenType::NewLine: return "NewLine";
        case TokenType::Unknown: return "Unknown";
    }
    return "Unknown";
}
