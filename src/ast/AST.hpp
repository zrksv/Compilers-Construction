#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../common/Location.hpp"

class AstNode {
public:
    Location location;

    virtual ~AstNode() = default;
};

class Expression : public AstNode {};
class Statement : public AstNode {};
class Declaration : public AstNode {};

class IntLiteral : public Expression {
public:
    int value;

    IntLiteral(int val, Location loc) : value(val) {
        this->location = loc;
    }
};

class RealLiteral : public Expression {
public:
    double value;

    RealLiteral(double val, Location loc) : value(val) {
        this->location = loc;
    }
};

class BoolLiteral : public Expression {
public:
    bool value;

    BoolLiteral(bool val, Location loc) : value(val) {
        this->location = loc;
    }
};

class Identifier : public Expression {
public:
    std::string name;

    Identifier(std::string val, Location loc) : name(std::move(val)) {
        this->location = loc;
    }
};

enum class UnaryOp {
    Not, Minus, Plus
};

class UnaryExpr : public Expression {
public:
    UnaryOp op;
    std::unique_ptr<Expression> child;

    UnaryExpr(UnaryOp op, std::unique_ptr<Expression> child, Location loc)
        : op(op), child(std::move(child)) {
        this->location = loc;
    }
};

enum class BinOp {
    And, Or, Xor,
    Plus, Minus, Mul, Div, Mod,
    Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual
};

class BinaryExpr : public Expression {
public:
    BinOp op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryExpr(BinOp op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, Location loc)
        : op(op), left(std::move(left)), right(std::move(right)) {
        this->location = loc;
    }
};

class RoutineCallExpr : public Expression {
public:
    std::string routine_name;
    std::vector<std::unique_ptr<Expression>> arguments;

    RoutineCallExpr(std::string name, std::vector<std::unique_ptr<Expression>> args, Location loc)
        : routine_name(std::move(name)), arguments(std::move(args)) {
        this->location = loc;
    }
};

class AssignStatement : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> value;

    AssignStatement(std::string name, std::unique_ptr<Expression> val, Location loc)
        : name(std::move(name)), value(std::move(val)) {
        this->location = loc;
    }
};

class RoutineCallStmt : public Statement {
public:
    std::string routine_name;
    std::vector<std::unique_ptr<Expression>> arguments;

    RoutineCallStmt(std::string name, std::vector<std::unique_ptr<Expression>> args, Location loc)
        : routine_name(std::move(name)), arguments(std::move(args)) {
        this->location = loc;
    }
};

class PrintStatement : public Statement {
public:
    std::vector<std::unique_ptr<Expression>> to_print;

    PrintStatement(std::vector<std::unique_ptr<Expression>> for_print, Location loc) 
        : to_print(std::move(for_print)) {
        this->location = loc;
    }
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<AstNode>> then_body;
    std::vector<std::unique_ptr<AstNode>> else_body;

    IfStatement(std::unique_ptr<Expression> cond,
                std::vector<std::unique_ptr<AstNode>> then_b,
                std::vector<std::unique_ptr<AstNode>> else_b,
                Location loc)
        : condition(std::move(cond)), then_body(std::move(then_b)), else_body(std::move(else_b)) {
        this->location = loc;
    }
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<AstNode>> body;

    WhileStatement(std::unique_ptr<Expression> cond, std::vector<std::unique_ptr<AstNode>> b, Location loc)
        : condition(std::move(cond)), body(std::move(b)) {
        this->location = loc;
    }
};

class ForStatement : public Statement {
public:
    std::string iterator_name;
    std::unique_ptr<Expression> range_start;
    std::unique_ptr<Expression> range_end;
    bool is_reverse;
    std::vector<std::unique_ptr<AstNode>> body;

    ForStatement(std::string iter,
                 std::unique_ptr<Expression> start,
                 std::unique_ptr<Expression> end,
                 bool rev,
                 std::vector<std::unique_ptr<AstNode>> b,
                 Location loc)
        : iterator_name(std::move(iter)), range_start(std::move(start)),
          range_end(std::move(end)), is_reverse(rev), body(std::move(b)) {
        this->location = loc;
    }
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;

    ReturnStatement(std::unique_ptr<Expression> expr, Location loc)
        : expression(std::move(expr)) {
        this->location = loc;
    }
};

class VariableDeclaration : public Declaration {
public:
    std::string name;
    std::string type;
    std::unique_ptr<Expression> expression;

    VariableDeclaration(std::string name, std::string type, std::unique_ptr<Expression> exp, Location loc) 
        : name(std::move(name)), type(std::move(type)), expression(std::move(exp)) {
        this->location = loc;
    }
};

class RoutineDeclaration : public Declaration {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::string return_type;
    std::vector<std::unique_ptr<AstNode>> body;

    RoutineDeclaration(std::string name,
                       std::vector<std::string> params,
                       std::string ret_type,
                       std::vector<std::unique_ptr<AstNode>> b,
                       Location loc)
        : name(std::move(name)), parameters(std::move(params)), return_type(std::move(ret_type)), body(std::move(b)) {
        this->location = loc;
    }
};

