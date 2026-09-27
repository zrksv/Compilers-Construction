#include "ParserDriver.hpp"
#include <fstream>
#include <sstream>

#include "Parser.hpp"

bool ParserDriver::parse(SourceManager& source_manager) {
    Lexer l(source_manager);
    return parse(l);
}

bool ParserDriver::parse(const std::string& source_code) {
    SourceManager sm(source_code);
    return parse(sm);
}

bool ParserDriver::parse_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        error(Location{1, 1}, "Cannot open file: " + filename);
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parse(buffer.str());
}


yy::Parser::symbol_type yylex(ParserDriver& driver) {
    Lexer* lex = driver.get_lexer();
    if (!lex) {
        return yy::Parser::make_YYEOF(Location{0, 0});
    }

    while (true) {
        Token tok = lex->next_token();

        switch (tok.type) {
            case TokenType::Eof:
                return yy::Parser::make_YYEOF(tok.location);

            case TokenType::NewLine:
                return yy::Parser::make_NEWLINE(tok.location);

            case TokenType::Semicolon:
                return yy::Parser::make_SEMICOLON(tok.location);

            case TokenType::Identifier:
                return yy::Parser::make_IDENTIFIER(tok.text, tok.location);

            case TokenType::IntegerLiteral: {
                try {
                    int val = std::stoi(tok.text);
                    return yy::Parser::make_INT_LITERAL(val, tok.location);
                } catch (...) {
                    driver.error(tok.location, "Invalid integer literal: " + tok.text);
                    return yy::Parser::make_INT_LITERAL(0, tok.location);
                }
            }

            case TokenType::RealLiteral: {
                try {
                    double val = std::stod(tok.text);
                    return yy::Parser::make_REAL_LITERAL(val, tok.location);
                } catch (...) {
                    driver.error(tok.location, "Invalid real literal: " + tok.text);
                    return yy::Parser::make_REAL_LITERAL(0.0, tok.location);
                }
            }

            case TokenType::True:
                return yy::Parser::make_TRUE(tok.location);
            case TokenType::False:
                return yy::Parser::make_FALSE(tok.location);

            case TokenType::Var:
                return yy::Parser::make_VAR(tok.location);
            case TokenType::Type:
                return yy::Parser::make_TYPE(tok.location);
            case TokenType::Routine:
                return yy::Parser::make_ROUTINE(tok.location);
            case TokenType::Is:
                return yy::Parser::make_IS(tok.location);
            case TokenType::End:
                return yy::Parser::make_END(tok.location);
            case TokenType::While:
                return yy::Parser::make_WHILE(tok.location);
            case TokenType::For:
                return yy::Parser::make_FOR(tok.location);
            case TokenType::Loop:
                return yy::Parser::make_LOOP(tok.location);
            case TokenType::Reverse:
                return yy::Parser::make_REVERSE(tok.location);
            case TokenType::In:
                return yy::Parser::make_IN(tok.location);
            case TokenType::If:
                return yy::Parser::make_IF(tok.location);
            case TokenType::Then:
                return yy::Parser::make_THEN(tok.location);
            case TokenType::Else:
                return yy::Parser::make_ELSE(tok.location);
            case TokenType::Print:
                return yy::Parser::make_PRINT(tok.location);
            case TokenType::Return:
                return yy::Parser::make_RETURN(tok.location);
            case TokenType::Size:
                return yy::Parser::make_SIZE(tok.location);

            case TokenType::Integer:
                return yy::Parser::make_KW_INTEGER(tok.location);
            case TokenType::Real:
                return yy::Parser::make_KW_REAL(tok.location);
            case TokenType::Bool:
                return yy::Parser::make_KW_BOOLEAN(tok.location);
            case TokenType::Array:
                return yy::Parser::make_ARRAY(tok.location);
            case TokenType::Record:
                return yy::Parser::make_RECORD(tok.location);

            case TokenType::And:
                return yy::Parser::make_AND(tok.location);
            case TokenType::Or:
                return yy::Parser::make_OR(tok.location);
            case TokenType::Xor:
                return yy::Parser::make_XOR(tok.location);
            case TokenType::Not:
                return yy::Parser::make_NOT(tok.location);

            case TokenType::Plus:
                return yy::Parser::make_PLUS(tok.location);
            case TokenType::Minus:
                return yy::Parser::make_MINUS(tok.location);
            case TokenType::Mul:
                return yy::Parser::make_MUL(tok.location);
            case TokenType::Div:
                return yy::Parser::make_DIV(tok.location);
            case TokenType::Mod:
                return yy::Parser::make_MOD(tok.location);

            case TokenType::Less:
                return yy::Parser::make_LESS(tok.location);
            case TokenType::LessEqual:
                return yy::Parser::make_LESS_EQUAL(tok.location);
            case TokenType::Greater:
                return yy::Parser::make_GREATER(tok.location);
            case TokenType::GreaterEqual:
                return yy::Parser::make_GREATER_EQUAL(tok.location);
            case TokenType::Equal:
                return yy::Parser::make_EQUAL(tok.location);
            case TokenType::NotEqual:
                return yy::Parser::make_NOT_EQUAL(tok.location);

            case TokenType::Assign:
                return yy::Parser::make_ASSIGN(tok.location);
            case TokenType::Range:
                return yy::Parser::make_RANGE(tok.location);
            case TokenType::Arrow:
                return yy::Parser::make_ARROW(tok.location);

            case TokenType::LeftPar:
                return yy::Parser::make_LPAREN(tok.location);
            case TokenType::RightPar:
                return yy::Parser::make_RPAREN(tok.location);
            case TokenType::LeftBr:
                return yy::Parser::make_LBRACKET(tok.location);
            case TokenType::RightBr:
                return yy::Parser::make_RBRACKET(tok.location);
            case TokenType::Colon:
                return yy::Parser::make_COLON(tok.location);
            case TokenType::Comma:
                return yy::Parser::make_COMMA(tok.location);
            case TokenType::Dot:
                return yy::Parser::make_DOT(tok.location);

            case TokenType::Unknown:
            default:
                driver.error(tok.location, "Unknown or unexpected token: '" + tok.text + "'");
                break;
        }
    }
}

bool ParserDriver::parse(Lexer& lex) {
    clear_errors();
    ast_root.reset();
    this->lexer = &lex;

    yy::Parser parser(*this);
    int res = parser.parse();
    this->lexer = nullptr;
    return (res == 0) && !has_errors();
}
