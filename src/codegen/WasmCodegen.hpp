#pragma once

#include "ASTVisitor.hpp"
#include "WatWriter.hpp"
#include "WasmStorage.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class ConversionKind;
struct RoutineSymbol;
class AstNode;

namespace WasmHost {
inline constexpr char print_i32[] = "$print_i32";
inline constexpr char print_f64[] = "$print_f64";
inline constexpr char print_bool[] = "$print_bool";
inline constexpr char print_space[] = "$print_space";
inline constexpr char print_newline[] = "$print_newline";
inline constexpr char report_trap[] = "$report_trap";
}

class WasmCodegen : public AstVisitor {
private:
    struct FunctionState {
        const RoutineSymbol* routine = nullptr;
        const Type* result_type = nullptr;
        std::uint32_t next_parameter = 0;
        std::uint32_t next_local = 0;
        std::uint32_t next_label = 0;
    } function_;

    WatWriter writer_;
    WasmStorage storage_;
    const SemanticModel* model_ = nullptr;
    std::unordered_map<const RoutineSymbol*, std::string> function_ids_;

    const SemanticModel& model() const;
    void prepare(Program& program, const SemanticModel& model);
    const std::string& function_id(const RoutineSymbol& routine) const;
    void begin_function_state(const RoutineSymbol* routine = nullptr);
    std::string new_label();
    void emit_body(const std::vector<std::unique_ptr<AstNode>>& body);

    void emit_initialized_value(Expression& expression);
    std::string capture_expr(const Expression& expression, bool convert);
    void append_storage(const std::vector<std::string>& locals, const std::vector<std::string>& instructions);
    Place emit_place(Expression& expression);
    void emit_load(Expression& expression);
    void emit_globals(Program& program);

public:
    std::string generate_wat(Program& program, const SemanticModel& model);

    void emit_expr(Expression& expression);
    void emit_conversion(ConversionKind conversion);
    void emit_call(const RoutineSymbol& routine, const std::vector<Expression*>& arguments);

    void emit_trap(const Location& location, WasmTrapKind kind);
    void visit(IntLiteral& node) override;
    void visit(RealLiteral& node) override;
    void visit(BoolLiteral& node) override;
    void visit(UnaryExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(RoutineCallExpr& node) override;
    void visit(RoutineCallStmt& node) override;
    void visit(PrintStatement& node) override;

    void visit(Program& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(ReturnStatement& node) override;

    void visit(Identifier& node) override;
    void visit(FieldAccessExpr& node) override;
    void visit(ElementAccessExpr& node) override;
    void visit(VariableDeclaration& node) override;
    void visit(AssignStatement& node) override;
    void visit(ParameterDeclaration& node) override;
    void visit(TypeDeclaration& node) override;
    void visit(PrimitiveTypeNode& node) override;
    void visit(IdentifierTypeNode& node) override;
    void visit(ArrayTypeNode& node) override;
    void visit(RecordTypeNode& node) override;
};
