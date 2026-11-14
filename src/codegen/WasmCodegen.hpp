#pragma once

#include "WatWriter.hpp"
#include "WasmStorage.hpp"

enum class ConversionKind;
struct RoutineSymbol;

class WasmCodegen {
private:
    WatWriter writer_;
    WasmStorage storage_;
    const SemanticModel* model_ = nullptr;
    std::unordered_map<const RoutineSymbol*, std::string> function_ids_;

public:
    std::string generate_wat(Program& program, const SemanticModel& model);

    void emit_expr(Expression& expression);
    void emit_conversion(ConversionKind conversion);
    void emit_call(const RoutineSymbol& routine, const std::vector<Expression*>& arguments);
};
