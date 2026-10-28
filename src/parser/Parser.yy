%skeleton "lalr1.cc"
%require "3.8"
%defines
%locations
%define api.parser.class {Parser}
%define api.namespace {yy}
%define api.token.constructor
%define api.value.type variant
%define api.value.automove
%define api.location.type {Location}
%define parse.assert
%define parse.error verbose
%define parse.lac full
%define lr.default-reduction accepting

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

%precedence BLOCK_END
%precedence error
%left OR AND XOR
%nonassoc LESS LESS_EQUAL GREATER GREATER_EQUAL EQUAL NOT_EQUAL
%left PLUS MINUS
%left MUL DIV MOD
%precedence UMINUS UPLUS NOT

%nterm <std::unique_ptr<Expression>> expression modifiable_primary
%nterm <std::vector<std::unique_ptr<Expression>>> expression_list argument_list
%nterm <std::unique_ptr<Statement>> statement
%nterm <std::unique_ptr<Declaration>> declaration
%nterm <std::unique_ptr<VariableDeclaration>> variable_declaration record_field
%nterm <std::unique_ptr<TypeDeclaration>> type_declaration
%nterm <std::unique_ptr<RoutineDeclaration>> routine_declaration
%nterm <std::unique_ptr<TypeNode>> type_spec primitive_type array_type record_type optional_return_type
%nterm <std::vector<std::unique_ptr<VariableDeclaration>>> record_fields
%nterm <std::vector<std::unique_ptr<AstNode>>> body body_items top_items
%nterm <std::unique_ptr<AstNode>> body_item top_item
%nterm <std::vector<std::unique_ptr<ParameterDeclaration>>> parameter_list parameters
%nterm <std::unique_ptr<ParameterDeclaration>> parameter
%nterm <std::string> field_name
%nterm <bool> optional_reverse

%%

program:
    opt_seps
    {
        driver.set_ast_root(std::make_unique<Program>(std::vector<std::unique_ptr<AstNode>>{}, Location{}));
    }
  | top_items opt_seps
    {
        driver.set_ast_root(std::make_unique<Program>($1, Location{}));
    }
  | seps top_items opt_seps
    {
        driver.set_ast_root(std::make_unique<Program>($2, Location{}));
    }
  ;

top_items:
    top_item
    {
        std::vector<std::unique_ptr<AstNode>> items;
        if (auto item = $1) items.push_back(std::move(item));
        $$ = std::move(items);
    }
  | top_items seps top_item
    {
        auto items = $1;
        if (auto item = $3) items.push_back(std::move(item));
        $$ = std::move(items);
    }
  | top_items error
    {
        yyerrok;
        $$ = $1;
    }
  ;

top_item:
    routine_declaration
    {
        $$ = $1;
    }
  | declaration
    {
        $$ = $1;
    }
  | error
    {
        yyerrok;
        $$ = nullptr;
    }
  ;

routine_declaration:
    ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type IS body block_end
    {
        $$ = std::make_unique<RoutineDeclaration>($2, $4, $6, $8, @1);
    }
  | ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type ARROW expression
    {
        $$ = std::make_unique<RoutineDeclaration>($2, $4, $6, $8, @1);
    }
  | ROUTINE IDENTIFIER LPAREN parameter_list RPAREN optional_return_type
    {
        $$ = std::make_unique<RoutineDeclaration>($2, $4, $6, @1);
    }
  | ROUTINE error IS body block_end
    {
        $$ = nullptr;
    }
  | ROUTINE error ARROW expression
    {
        $$ = nullptr;
    }
  | ROUTINE error
    {
        $$ = nullptr;
    }
  ;

parameter_list:
    %empty
    {
        $$ = std::vector<std::unique_ptr<ParameterDeclaration>>{};
    }
  | parameters
    {
        $$ = $1;
    }
  ;

parameters:
    parameter
    {
        std::vector<std::unique_ptr<ParameterDeclaration>> params;
        params.push_back($1);
        $$ = std::move(params);
    }
  | parameters COMMA parameter
    {
        auto params = $1;
        params.push_back($3);
        $$ = std::move(params);
    }
  ;

parameter:
    IDENTIFIER COLON type_spec
    {
        $$ = std::make_unique<ParameterDeclaration>($1, $3, @1);
    }
  ;

optional_return_type:
    %empty
    {
        $$ = nullptr;
    }
  | COLON type_spec
    {
        $$ = $2;
    }
  ;

declaration:
    variable_declaration
    {
        $$ = $1;
    }
  | type_declaration
    {
        $$ = $1;
    }
  ;

type_declaration:
    TYPE IDENTIFIER IS type_spec
    {
        $$ = std::make_unique<TypeDeclaration>($2, $4, @1);
    }
  ;

type_spec:
    primitive_type
    {
        $$ = $1;
    }
  | IDENTIFIER
    {
        $$ = std::make_unique<IdentifierTypeNode>($1, @1);
    }
  | array_type
    {
        $$ = $1;
    }
  | record_type
    {
        $$ = $1;
    }
  ;

primitive_type:
    KW_INTEGER { $$ = std::make_unique<PrimitiveTypeNode>("integer", @1); }
  | KW_REAL    { $$ = std::make_unique<PrimitiveTypeNode>("real", @1); }
  | KW_BOOLEAN { $$ = std::make_unique<PrimitiveTypeNode>("boolean", @1); }
  ;

array_type:
    ARRAY LBRACKET expression RBRACKET type_spec
    {
        $$ = std::make_unique<ArrayTypeNode>($3, $5, @1);
    }
  | ARRAY LBRACKET RBRACKET type_spec
    {
        $$ = std::make_unique<ArrayTypeNode>(nullptr, $4, @1);
    }
  ;

record_type:
    RECORD record_fields opt_seps block_end
    {
        $$ = std::make_unique<RecordTypeNode>($2, @1);
    }
  | RECORD seps record_fields opt_seps block_end
    {
        $$ = std::make_unique<RecordTypeNode>($3, @1);
    }
  | RECORD opt_seps block_end
    {
        $$ = std::make_unique<RecordTypeNode>(std::vector<std::unique_ptr<VariableDeclaration>>{}, @1);
    }
  ;

record_fields:
    record_field
    {
        std::vector<std::unique_ptr<VariableDeclaration>> fields;
        if (auto field = $1) fields.push_back(std::move(field));
        $$ = std::move(fields);
    }
  | record_fields seps record_field
    {
        auto fields = $1;
        if (auto field = $3) fields.push_back(std::move(field));
        $$ = std::move(fields);
    }
  | record_fields error
    {
        yyerrok;
        $$ = $1;
    }
  ;

record_field:
    variable_declaration
    {
        $$ = $1;
    }
  | error
    {
        yyerrok;
        $$ = nullptr;
    }
  ;

variable_declaration:
    VAR IDENTIFIER COLON type_spec IS expression
    {
        $$ = std::make_unique<VariableDeclaration>($2, $4, $6, @1);
    }
  | VAR IDENTIFIER COLON type_spec
    {
        $$ = std::make_unique<VariableDeclaration>($2, $4, nullptr, @1);
    }
  | VAR IDENTIFIER IS expression
    {
        $$ = std::make_unique<VariableDeclaration>($2, nullptr, $4, @1);
    }
  ;

body:
    opt_seps
    {
        $$ = std::vector<std::unique_ptr<AstNode>>{};
    }
  | body_items opt_seps
    {
        $$ = $1;
    }
  | seps body_items opt_seps
    {
        $$ = $2;
    }
  ;

body_items:
    body_item
    {
        std::vector<std::unique_ptr<AstNode>> items;
        if (auto item = $1) items.push_back(std::move(item));
        $$ = std::move(items);
    }
  | body_items seps body_item
    {
        auto items = $1;
        if (auto item = $3) items.push_back(std::move(item));
        $$ = std::move(items);
    }
  | body_items error
    {
        yyerrok;
        $$ = $1;
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
  | error
    {
        yyerrok;
        $$ = nullptr;
    }
  ;

statement:
    modifiable_primary ASSIGN expression
    {
        $$ = std::make_unique<AssignStatement>($1, $3, @2);
    }
  | IDENTIFIER LPAREN argument_list RPAREN
    {
        $$ = std::make_unique<RoutineCallStmt>($1, $3, @1);
    }
  | PRINT expression_list
    {
        $$ = std::make_unique<PrintStatement>($2, @1);
    }
  | IF expression THEN body block_end
    {
        $$ = std::make_unique<IfStatement>($2, $4, std::vector<std::unique_ptr<AstNode>>{}, @1);
    }
  | IF expression THEN body ELSE body block_end
    {
        $$ = std::make_unique<IfStatement>($2, $4, $6, @1);
    }
  | WHILE expression LOOP body block_end
    {
        $$ = std::make_unique<WhileStatement>($2, $4, @1);
    }
  | FOR IDENTIFIER IN expression RANGE expression optional_reverse LOOP body block_end
    {
        $$ = std::make_unique<ForStatement>($2, $4, $6, $7, $9, @1);
    }
  | FOR IDENTIFIER IN expression optional_reverse LOOP body block_end
    {
        $$ = std::make_unique<ForStatement>($2, $4, $5, $7, @1);
    }
  | IF error THEN body block_end
    {
        $$ = nullptr;
    }
  | IF error THEN body ELSE body block_end
    {
        $$ = nullptr;
    }
  | IF error sep body block_end
    {
        $$ = nullptr;
    }
  | IF error sep body ELSE body block_end
    {
        $$ = nullptr;
    }
  | WHILE error LOOP body block_end
    {
        $$ = nullptr;
    }
  | WHILE error sep body block_end
    {
        $$ = nullptr;
    }
  | FOR error LOOP body block_end
    {
        $$ = nullptr;
    }
  | FOR error sep body block_end
    {
        $$ = nullptr;
    }
  | RETURN
    {
        $$ = std::make_unique<ReturnStatement>(nullptr, @1);
    }
  | RETURN expression
    {
        $$ = std::make_unique<ReturnStatement>($2, @1);
    }
  ;

block_end:
    END %prec BLOCK_END
  | END error
    {
        yyerrok;
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

modifiable_primary:
    IDENTIFIER
    {
        $$ = std::make_unique<Identifier>($1, @1);
    }
  | modifiable_primary DOT field_name opt_empty_parens
    {
        $$ = std::make_unique<FieldAccessExpr>($1, $3, @2);
    }
  | modifiable_primary LBRACKET expression RBRACKET
    {
        $$ = std::make_unique<ElementAccessExpr>($1, $3, @2);
    }
  ;

opt_empty_parens:
    %empty
  | LPAREN RPAREN
  ;

field_name:
    IDENTIFIER { $$ = $1; }
  | SIZE       { $$ = "size"; }
  ;

expression:
    INT_LITERAL
    {
        $$ = std::make_unique<IntLiteral>($1, @1);
    }
  | REAL_LITERAL
    {
        $$ = std::make_unique<RealLiteral>($1, @1);
    }
  | TRUE
    {
        $$ = std::make_unique<BoolLiteral>(true, @1);
    }
  | FALSE
    {
        $$ = std::make_unique<BoolLiteral>(false, @1);
    }
  | modifiable_primary
    {
        $$ = $1;
    }
  | LPAREN expression RPAREN
    {
        $$ = $2;
    }
  | expression PLUS expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Plus, $1, $3, @2);
    }
  | expression MINUS expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Minus, $1, $3, @2);
    }
  | expression MUL expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Mul, $1, $3, @2);
    }
  | expression DIV expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Div, $1, $3, @2);
    }
  | expression MOD expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Mod, $1, $3, @2);
    }
  | expression AND expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::And, $1, $3, @2);
    }
  | expression OR expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Or, $1, $3, @2);
    }
  | expression XOR expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Xor, $1, $3, @2);
    }
  | expression LESS expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Less, $1, $3, @2);
    }
  | expression LESS_EQUAL expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::LessEqual, $1, $3, @2);
    }
  | expression GREATER expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Greater, $1, $3, @2);
    }
  | expression GREATER_EQUAL expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::GreaterEqual, $1, $3, @2);
    }
  | expression EQUAL expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::Equal, $1, $3, @2);
    }
  | expression NOT_EQUAL expression
    {
        $$ = std::make_unique<BinaryExpr>(BinOp::NotEqual, $1, $3, @2);
    }
  | PLUS expression %prec UPLUS
    {
        $$ = std::make_unique<UnaryExpr>(UnaryOp::Plus, $2, @1);
    }
  | MINUS expression %prec UMINUS
    {
        $$ = std::make_unique<UnaryExpr>(UnaryOp::Minus, $2, @1);
    }
  | NOT expression %prec NOT
    {
        $$ = std::make_unique<UnaryExpr>(UnaryOp::Not, $2, @1);
    }
  | IDENTIFIER LPAREN argument_list RPAREN
    {
        $$ = std::make_unique<RoutineCallExpr>($1, $3, @1);
    }
  ;

argument_list:
    %empty
    {
        $$ = std::vector<std::unique_ptr<Expression>>{};
    }
  | expression_list
    {
        $$ = $1;
    }
  ;

expression_list:
    expression
    {
        std::vector<std::unique_ptr<Expression>> list;
        list.push_back($1);
        $$ = std::move(list);
    }
  | expression_list COMMA expression
    {
        auto list = $1;
        list.push_back($3);
        $$ = std::move(list);
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
