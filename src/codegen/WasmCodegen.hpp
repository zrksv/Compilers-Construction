#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

struct VariableSymbol;
class Identifier;
class SemanticModel;
class Expression;
class VariableDeclaration;
class AssignStatement;
class Program;

enum class PlaceKind {
    Local,
    Global
};

struct Place {
    PlaceKind kind;
    std::string wat_name;
};

class WasmCodegen {
private:
    std::unordered_map<const VariableSymbol*, Place> locals_;
    std::unordered_map<const VariableSymbol*, Place> globals_;

public:
    void begin_function();
    void bind_local(const VariableSymbol& symbol, const std::string& wat_name);
    void bind_global(const VariableSymbol& symbol, const std::string& wat_name);
    const Place* find_place(const VariableSymbol& symbol) const;
    const Place* place_of(const Identifier& identifier, const SemanticModel& model) const;

    static std::string wat_value_type(const VariableSymbol& symbol);
    static std::string local_declaration(const VariableSymbol& symbol, const std::string& wat_name);
    static std::string global_declaration(const VariableSymbol& symbol, const std::string& wat_name);
    static std::string load_instruction(const Place& place);
    static std::string store_instruction(const Place& place);
    static std::string default_value_instruction(const VariableSymbol& symbol);

    void emit_local_variable(const VariableDeclaration& declaration, const SemanticModel& model,
        const std::string& wat_name, std::vector<std::string>& local_declarations,
        std::vector<std::string>& instructions, const std::function<std::string(const Expression&)>& emit_initializer);
    void emit_assignment(const AssignStatement& assignment, const SemanticModel& model,
        std::vector<std::string>& instructions, const std::function<std::string(const Expression&)>& emit_value);
    void emit_global_variable(const VariableDeclaration& declaration, const SemanticModel& model,
        const std::string& wat_name, std::vector<std::string>& global_declarations,
        std::vector<std::string>& init_instructions, const std::function<std::string(const Expression&)>& emit_initializer);
    void emit_globals(const Program& program, const SemanticModel& model, std::vector<std::string>& global_declarations,
        std::vector<std::string>& init_instructions, const std::function<std::string(const Expression&)>& emit_initializer);
};
