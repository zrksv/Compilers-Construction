#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../common/Location.hpp"

class AstPrinter;

class AstNode {
public:
    Location location;

    virtual ~AstNode() = default;

    virtual void accept(AstPrinter& printer) = 0;
};

class Expression : public AstNode {};
class Statement : public AstNode {};
class Declaration : public AstNode {};
class TypeNode : public AstNode {};

class Program : public AstNode {
public:
    std::vector<std::unique_ptr<AstNode>> declarations;

    Program(std::vector<std::unique_ptr<AstNode>> decls, Location loc)
        : declarations(std::move(decls)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

// ============================================================================
// Expressions
// ============================================================================

class IntLiteral : public Expression {
public:
    int value;

    IntLiteral(int val, Location loc) : value(val) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class RealLiteral : public Expression {
public:
    double value;

    RealLiteral(double val, Location loc) : value(val) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class BoolLiteral : public Expression {
public:
    bool value;

    BoolLiteral(bool val, Location loc) : value(val) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class Identifier : public Expression {
public:
    std::string name;

    Identifier(std::string val, Location loc) : name(std::move(val)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class FieldAccessExpr : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::string field_name;
    FieldAccessExpr(std::unique_ptr<Expression> object, std::string field_name, Location loc)
        : object(std::move(object)), field_name(std::move(field_name)) {
        this->location = loc;
    }
    void accept(AstPrinter& printer) override;
};

class ElementAccessExpr : public Expression {
public:
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;

    ElementAccessExpr(std::unique_ptr<Expression> array, std::unique_ptr<Expression> index, Location loc)
        : array(std::move(array)), index(std::move(index)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
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

    void accept(AstPrinter& printer) override;
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

    void accept(AstPrinter& printer) override;
};

class RoutineCallExpr : public Expression {
public:
    std::string routine_name;
    std::vector<std::unique_ptr<Expression>> arguments;

    RoutineCallExpr(std::string name, std::vector<std::unique_ptr<Expression>> args, Location loc)
        : routine_name(std::move(name)), arguments(std::move(args)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

// ============================================================================
// Statements
// ============================================================================

class AssignStatement : public Statement {
public:
    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;

    AssignStatement(std::unique_ptr<Expression> target, std::unique_ptr<Expression> val, Location loc)
        : target(std::move(target)), value(std::move(val)) {
        this->location = loc;
    }

    AssignStatement(const std::string& name, std::unique_ptr<Expression> val, Location loc)
        : target(std::make_unique<Identifier>(name, loc)), value(std::move(val)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class RoutineCallStmt : public Statement {
public:
    std::string routine_name;
    std::vector<std::unique_ptr<Expression>> arguments;

    RoutineCallStmt(std::string name, std::vector<std::unique_ptr<Expression>> args, Location loc)
        : routine_name(std::move(name)), arguments(std::move(args)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class PrintStatement : public Statement {
public:
    std::vector<std::unique_ptr<Expression>> to_print;

    PrintStatement(std::vector<std::unique_ptr<Expression>> for_print, Location loc) 
        : to_print(std::move(for_print)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
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

    void accept(AstPrinter& printer) override;
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<AstNode>> body;

    WhileStatement(std::unique_ptr<Expression> cond, std::vector<std::unique_ptr<AstNode>> b, Location loc)
        : condition(std::move(cond)), body(std::move(b)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
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

    // For loop iterating over an array (range_end is nullptr)
    ForStatement(std::string iter,
                 std::unique_ptr<Expression> array_expr,
                 bool rev,
                 std::vector<std::unique_ptr<AstNode>> b,
                 Location loc)
        : iterator_name(std::move(iter)), range_start(std::move(array_expr)),
          range_end(nullptr), is_reverse(rev), body(std::move(b)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;

    ReturnStatement(std::unique_ptr<Expression> expr, Location loc)
        : expression(std::move(expr)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

// ============================================================================
// Type Nodes
// ============================================================================

class PrimitiveTypeNode : public TypeNode {
public:
    std::string type_name;

    PrimitiveTypeNode(std::string name, Location loc) : type_name(std::move(name)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class IdentifierTypeNode : public TypeNode {
public:
    std::string name;

    IdentifierTypeNode(std::string name, Location loc) : name(std::move(name)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class ArrayTypeNode : public TypeNode {
public:
    std::unique_ptr<Expression> size; // nullptr for unsized array in parameters (array [] T)
    std::unique_ptr<TypeNode> element_type;

    ArrayTypeNode(std::unique_ptr<Expression> size, std::unique_ptr<TypeNode> element_type, Location loc)
        : size(std::move(size)), element_type(std::move(element_type)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

// ============================================================================
// Declarations
// ============================================================================

class ParameterDeclaration : public AstNode {
public:
    std::string name;
    std::unique_ptr<TypeNode> type;

    ParameterDeclaration(std::string name, std::unique_ptr<TypeNode> type, Location loc)
        : name(std::move(name)), type(std::move(type)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class VariableDeclaration : public Declaration {
public:
    std::string name;
    std::unique_ptr<TypeNode> type;
    std::unique_ptr<Expression> expression;

    VariableDeclaration(std::string name, std::unique_ptr<TypeNode> type, std::unique_ptr<Expression> exp, Location loc)
        : name(std::move(name)), type(std::move(type)), expression(std::move(exp)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class RecordTypeNode : public TypeNode {
public:
    std::vector<std::unique_ptr<VariableDeclaration>> declarations;

    RecordTypeNode(std::vector<std::unique_ptr<VariableDeclaration>> declarations, Location loc)
        : declarations(std::move(declarations)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

enum class RoutineBodyType {
    Block,
    Expression,
    Forward
};

class RoutineDeclaration : public Declaration {
public:
    std::string name;
    std::vector<std::unique_ptr<ParameterDeclaration>> parameters;
    std::unique_ptr<TypeNode> return_type;

    RoutineBodyType body_type;
    std::vector<std::unique_ptr<AstNode>> body;
    std::unique_ptr<Expression> expression_body;

    // Block declaration
    RoutineDeclaration(std::string name,
                       std::vector<std::unique_ptr<ParameterDeclaration>> params,
                       std::unique_ptr<TypeNode> ret_type,
                       std::vector<std::unique_ptr<AstNode>> b,
                       Location loc)
        : name(std::move(name)), parameters(std::move(params)), return_type(std::move(ret_type)),
          body_type(RoutineBodyType::Block), body(std::move(b)), expression_body(nullptr) {
        this->location = loc;
    }

    // Forward declaration
    RoutineDeclaration(std::string name,
                       std::vector<std::unique_ptr<ParameterDeclaration>> params,
                       std::unique_ptr<TypeNode> ret_type,
                       Location loc)
        : name(std::move(name)), parameters(std::move(params)), return_type(std::move(ret_type)),
          body_type(RoutineBodyType::Forward), expression_body(nullptr) {
        this->location = loc;
    }

    // Arrow expression (=> Expr)
    RoutineDeclaration(std::string name,
                       std::vector<std::unique_ptr<ParameterDeclaration>> params,
                       std::unique_ptr<TypeNode> ret_type,
                       std::unique_ptr<Expression> expr,
                       Location loc)
        : name(std::move(name)), parameters(std::move(params)), return_type(std::move(ret_type)),
          body_type(RoutineBodyType::Expression), expression_body(std::move(expr)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};

class TypeDeclaration : public Declaration {
public:
    std::string name;
    std::unique_ptr<TypeNode> type;

    TypeDeclaration(std::string name, std::unique_ptr<TypeNode> type, Location loc)
        : name(std::move(name)), type(std::move(type)) {
        this->location = loc;
    }

    void accept(AstPrinter& printer) override;
};