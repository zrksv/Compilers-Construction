#pragma once

#include <string>
#include <string_view>

class WatWriter {
public:
    void module_item(std::string_view wat);
    void begin_function(std::string_view header);
    void add_local(std::string_view name, std::string_view wat_type);
    std::string temporary(std::string_view wat_type);
    void instruction(std::string_view wat);
    void end_function();
    std::string finish_module() const;
};
