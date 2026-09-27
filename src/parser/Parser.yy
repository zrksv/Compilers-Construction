%skeleton "lalr1.cc"
%require "3.5"
%defines
%locations
%define api.parser.class {Parser}
%define api.namespace {yy}
%define api.token.constructor
%define api.value.type variant
%define api.location.type {Location}
%define parse.assert
%define parse.error verbose

%code requires {
#include <string>
#include <vector>
#include <memory>
#include "../common/Location.hpp"
#include "../ast/AST.hpp"

#define YYLLOC_DEFAULT(Cur, Rhs, N) \
    do { \
        if (N) (Cur) = YYRHSLOC(Rhs, 1); \
        else   (Cur) = YYRHSLOC(Rhs, 0); \
    } while (0)

class ParserDriver;
}

%param { ParserDriver& driver }

%code {
#include "ParserDriver.hpp"

yy::Parser::symbol_type yylex(ParserDriver& driver);

void yy::Parser::error(const Location& loc, const std::string& msg) {
    driver.error(loc, msg);
}
}

%token <std::string> IDENTIFIER
%token <int> INT_LITERAL
%token <double> REAL_LITERAL
%token TRUE FALSE

%token VAR TYPE ROUTINE IS END
%token WHILE FOR LOOP REVERSE IN
%token IF THEN ELSE
%token PRINT RETURN SIZE
%token KW_INTEGER KW_REAL KW_BOOLEAN
%token ARRAY RECORD

%token AND OR XOR NOT
%token PLUS MINUS MUL DIV MOD
%token LESS LESS_EQUAL GREATER GREATER_EQUAL EQUAL NOT_EQUAL
%token ASSIGN RANGE ARROW

%token LPAREN RPAREN LBRACKET RBRACKET COLON COMMA DOT
%token SEMICOLON NEWLINE

%left OR AND XOR
%nonassoc LESS LESS_EQUAL GREATER GREATER_EQUAL EQUAL NOT_EQUAL
%left PLUS MINUS
%left MUL DIV MOD
%precedence UMINUS UPLUS NOT

%nterm <Expression*> expression
%nterm <std::vector<Expression*>> expression_list argument_list
%nterm <Statement*> statement
%nterm <Declaration*> declaration
%nterm <VariableDeclaration*> variable_declaration
%nterm <RoutineDeclaration*> routine_declaration
%nterm <std::vector<AstNode*>> body body_items
%nterm <AstNode*> body_item
%nterm <std::vector<std::string>> parameter_list parameters
%nterm <std::string> parameter type_name optional_return_type
%nterm <bool> optional_reverse

%%

program:
    opt_seps top_items opt_seps
  | opt_seps
  ;

top_items:
    top_item
  | top_items seps top_item
  ;

top_item:
    routine_declaration
    {
        driver.set_ast_root(std::unique_ptr<AstNode>($1));
    }
  | declaration
    {
        driver.set_ast_root(std::unique_ptr<AstNode>($1));
    }
  ;

routine_declaration:
    ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type IS body END
    {
        std::vector<std::unique_ptr<AstNode>> body_vec;
        for (auto* item : $8) {
            body_vec.emplace_back(item);
        }
        $$ = new RoutineDeclaration($2, std::move($4), $6, std::move(body_vec), @1);
    }
  ;

parameter_list:
    %empty
    {
        $$ = std::vector<std::string>{};
    }
  | parameters
    {
        $$ = std::move($1);
    }
  ;

parameters:
    parameter
    {
        std::vector<std::string> params;
        params.push_back($1);
        $$ = std::move(params);
    }
  | parameters COMMA parameter
    {
        $1.push_back($3);
        $$ = std::move($1);
    }
  ;

parameter:
    IDENTIFIER COLON type_name
    {
        $$ = $1 + " : " + $3;
    }
  | IDENTIFIER
    {
        $$ = $1;
    }
  ;

optional_return_type:
    %empty
    {
        $$ = "";
    }
  | COLON type_name
    {
        $$ = $2;
    }
  ;

declaration:
    variable_declaration
    {
        $$ = $1;
    }
  ;

variable_declaration:
    VAR IDENTIFIER COLON type_name IS expression
    {
        $$ = new VariableDeclaration($2, $4, std::unique_ptr<Expression>($6), @1);
    }
  | VAR IDENTIFIER COLON type_name
    {
        $$ = new VariableDeclaration($2, $4, nullptr, @1);
    }
  | VAR IDENTIFIER IS expression
    {
        $$ = new VariableDeclaration($2, "", std::unique_ptr<Expression>($4), @1);
    }
  ;

type_name:
    KW_INTEGER { $$ = "integer"; }
  | KW_REAL    { $$ = "real"; }
  | KW_BOOLEAN { $$ = "boolean"; }
  | IDENTIFIER { $$ = $1; }
  ;

body:
    opt_seps
    {
        $$ = std::vector<AstNode*>{};
    }
  | opt_seps body_items opt_seps
    {
        $$ = std::move($2);
    }
  ;

body_items:
    body_item
    {
        std::vector<AstNode*> items;
        items.push_back($1);
        $$ = std::move(items);
    }
  | body_items seps body_item
    {
        $1.push_back($3);
        $$ = std::move($1);
    }
  ;

body_item:
    statement
    {
        $$ = $1;
    }
  | declaration
    {
        $$ = $1;
    }
  ;

statement:
    IDENTIFIER ASSIGN expression
    {
        $$ = new AssignStatement($1, std::unique_ptr<Expression>($3), @1);
    }
  | IDENTIFIER LPAREN argument_list RPAREN
    {
        std::vector<std::unique_ptr<Expression>> args;
        for (auto* arg : $3) {
            args.emplace_back(arg);
        }
        $$ = new RoutineCallStmt($1, std::move(args), @1);
    }
  | PRINT expression_list
    {
        std::vector<std::unique_ptr<Expression>> exprs;
        for (auto* e : $2) {
            exprs.emplace_back(e);
        }
        $$ = new PrintStatement(std::move(exprs), @1);
    }
  | IF expression THEN body END
    {
        std::vector<std::unique_ptr<AstNode>> then_body;
        for (auto* n : $4) {
            then_body.emplace_back(n);
        }
        $$ = new IfStatement(std::unique_ptr<Expression>($2), std::move(then_body), std::vector<std::unique_ptr<AstNode>>{}, @1);
    }
  | IF expression THEN body ELSE body END
    {
        std::vector<std::unique_ptr<AstNode>> then_body;
        for (auto* n : $4) {
            then_body.emplace_back(n);
        }
        std::vector<std::unique_ptr<AstNode>> else_body;
        for (auto* n : $6) {
            else_body.emplace_back(n);
        }
        $$ = new IfStatement(std::unique_ptr<Expression>($2), std::move(then_body), std::move(else_body), @1);
    }
  | WHILE expression LOOP body END
    {
        std::vector<std::unique_ptr<AstNode>> b;
        for (auto* n : $4) {
            b.emplace_back(n);
        }
        $$ = new WhileStatement(std::unique_ptr<Expression>($2), std::move(b), @1);
    }
  | FOR IDENTIFIER IN expression RANGE expression optional_reverse LOOP body END
    {
        std::vector<std::unique_ptr<AstNode>> b;
        for (auto* n : $9) {
            b.emplace_back(n);
        }
        $$ = new ForStatement($2, std::unique_ptr<Expression>($4), std::unique_ptr<Expression>($6), $7, std::move(b), @1);
    }
  | RETURN
    {
        $$ = new ReturnStatement(nullptr, @1);
    }
  | RETURN expression
    {
        $$ = new ReturnStatement(std::unique_ptr<Expression>($2), @1);
    }
  ;

optional_reverse:
    %empty
    {
        $$ = false;
    }
  | REVERSE
    {
        $$ = true;
    }
  ;

expression:
    INT_LITERAL
    {
        $$ = new IntLiteral($1, @1);
    }
  | REAL_LITERAL
    {
        $$ = new RealLiteral($1, @1);
    }
  | TRUE
    {
        $$ = new BoolLiteral(true, @1);
    }
  | FALSE
    {
        $$ = new BoolLiteral(false, @1);
    }
  | IDENTIFIER
    {
        $$ = new Identifier($1, @1);
    }
  | LPAREN expression RPAREN
    {
        $$ = $2;
    }
  | expression PLUS expression
    {
        $$ = new BinaryExpr(BinOp::Plus, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression MINUS expression
    {
        $$ = new BinaryExpr(BinOp::Minus, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression MUL expression
    {
        $$ = new BinaryExpr(BinOp::Mul, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression DIV expression
    {
        $$ = new BinaryExpr(BinOp::Div, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression MOD expression
    {
        $$ = new BinaryExpr(BinOp::Mod, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression AND expression
    {
        $$ = new BinaryExpr(BinOp::And, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression OR expression
    {
        $$ = new BinaryExpr(BinOp::Or, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression XOR expression
    {
        $$ = new BinaryExpr(BinOp::Xor, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression LESS expression
    {
        $$ = new BinaryExpr(BinOp::Less, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression LESS_EQUAL expression
    {
        $$ = new BinaryExpr(BinOp::LessEqual, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression GREATER expression
    {
        $$ = new BinaryExpr(BinOp::Greater, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression GREATER_EQUAL expression
    {
        $$ = new BinaryExpr(BinOp::GreaterEqual, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression EQUAL expression
    {
        $$ = new BinaryExpr(BinOp::Equal, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | expression NOT_EQUAL expression
    {
        $$ = new BinaryExpr(BinOp::NotEqual, std::unique_ptr<Expression>($1), std::unique_ptr<Expression>($3), @2);
    }
  | PLUS expression %prec UPLUS
    {
        $$ = new UnaryExpr(UnaryOp::Plus, std::unique_ptr<Expression>($2), @1);
    }
  | MINUS expression %prec UMINUS
    {
        $$ = new UnaryExpr(UnaryOp::Minus, std::unique_ptr<Expression>($2), @1);
    }
  | NOT expression %prec NOT
    {
        $$ = new UnaryExpr(UnaryOp::Not, std::unique_ptr<Expression>($2), @1);
    }
  | IDENTIFIER LPAREN argument_list RPAREN
    {
        std::vector<std::unique_ptr<Expression>> args;
        for (auto* arg : $3) {
            args.emplace_back(arg);
        }
        $$ = new RoutineCallExpr($1, std::move(args), @1);
    }
  ;

argument_list:
    %empty
    {
        $$ = std::vector<Expression*>{};
    }
  | expression_list
    {
        $$ = std::move($1);
    }
  ;

expression_list:
    expression
    {
        std::vector<Expression*> list;
        list.push_back($1);
        $$ = std::move(list);
    }
  | expression_list COMMA expression
    {
        $1.push_back($3);
        $$ = std::move($1);
    }
  ;

sep:
    SEMICOLON
  | NEWLINE
  ;

seps:
    sep
  | seps sep
  ;

opt_seps:
    %empty
  | seps
  ;

%%
