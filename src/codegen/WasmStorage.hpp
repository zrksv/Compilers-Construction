#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <cstdint>

struct VariableSymbol;
class Identifier;
class SemanticModel;
class Expression;
class VariableDeclaration;
class AssignStatement;
class Program;
class ArrayType;
class Type;
class ElementAccessExpr;

enum class PlaceKind {
    Local,
    Global,
    Memory
};

struct Place {
    PlaceKind kind;
    std::string wat_name;
    const Type* value_type = nullptr;
};

struct ArrayLayout {
    std::uint32_t elements_offset;
    std::uint32_t element_stride;
};

class WasmStorage {
private:
    std::unordered_map<const VariableSymbol*, Place> locals_;
    std::unordered_map<const VariableSymbol*, Place> globals_;
    std::uint32_t next_temporary_ = 0;
    std::string add_i32_temporary(std::vector<std::string>& local_declarations);

public:
    void begin_function();
    void bind_local(const VariableSymbol& symbol, const std::string& wat_name);
    void bind_global(const VariableSymbol& symbol, const std::string& wat_name);
    const Place* find_place(const VariableSymbol& symbol) const;
    const Place* place_of(const Identifier& identifier, const SemanticModel& model) const;
    Place emit_place(const Expression& expression, const SemanticModel& model,
        std::vector<std::string>& local_declarations, std::vector<std::string>& instructions,
        const std::function<std::string(const Expression&)>& emit_value);

    static std::string wat_value_type(const VariableSymbol& symbol);
    static std::string wat_value_type(const Type& type);
    static std::string memory_load_instruction(const Type& type);
    static std::string memory_store_instruction(const Type& type);
    static std::string local_declaration(const VariableSymbol& symbol, const std::string& wat_name);
    static std::string global_declaration(const VariableSymbol& symbol, const std::string& wat_name);
    static std::string load_instruction(const Place& place);
    static std::string store_instruction(const Place& place);
    static std::string default_value_instruction(const VariableSymbol& symbol);

    void emit_local_variable(const VariableDeclaration& declaration, const SemanticModel& model,
        const std::string& wat_name, std::vector<std::string>& local_declarations,
        std::vector<std::string>& instructions, const std::function<std::string(const Expression&)>& emit_initializer);
    void emit_assignment(const AssignStatement& assignment, const SemanticModel& model,
        std::vector<std::string>& local_declarations, std::vector<std::string>& instructions,
        const std::function<std::string(const Expression&)>& emit_value);
    void emit_global_variable(const VariableDeclaration& declaration, const SemanticModel& model,
        const std::string& wat_name, std::vector<std::string>& global_declarations,
        std::vector<std::string>& init_instructions, const std::function<std::string(const Expression&)>& emit_initializer);
    void emit_globals(const Program& program, const SemanticModel& model, std::vector<std::string>& global_declarations,
        std::vector<std::string>& init_instructions, const std::function<std::string(const Expression&)>& emit_initializer);

    static ArrayLayout array_layout(const ArrayType& array);
    static std::uint64_t array_allocation_size(const ArrayType& array);
    static void emit_primitive_array_creation(const ArrayType& array,
        const std::string& address_local, std::vector<std::string>& instructions);
    static void emit_array_size(std::vector<std::string>& instructions);
    static void emit_array_index_check(const std::string& address_local,
        const std::string& index_local, std::vector<std::string>& instructions);
    static void emit_array_element_address(const ArrayType& array, const std::string& address_local,
        const std::string& index_local, std::vector<std::string>& instructions);
    void emit_array_element_address(const ElementAccessExpr& access, const SemanticModel& model,
        std::vector<std::string>& local_declarations, std::vector<std::string>& instructions,
        const std::function<std::string(const Expression&)>& emit_value);
};
