#pragma once

#include <string>
#include <string_view>
#include <functional>

class WatWriter {
public:
    void reset();
    void module_item(std::string_view wat);
    // Header starts with the function ID, without an opening '(func'.
    void begin_function(std::string_view header);
    void add_local(std::string_view name, std::string_view wat_type);
    // Accepts a complete '(local $name type)' declaration from WasmStorage.
    void local_declaration(std::string_view wat);
    // Uses $writer_tmpN, separately from storage's $tmpN.
    std::string temporary(std::string_view wat_type);
    void instruction(std::string_view wat);
    // Captures instructions only; locals created by emit stay in the current function.
    std::string capture_instructions(const std::function<void()>& emit);
    void end_function();
    std::string finish_module() const;
};
