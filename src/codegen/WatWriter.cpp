#include "WatWriter.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
std::string indent(const std::string& text, unsigned level) {
    std::istringstream input(text);
    std::ostringstream output;
    const std::string padding(level * 2, ' ');
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty()) {
            output << padding << line << '\n';
        }
    }
    return output.str();
}
}

void WatWriter::reset() {
    items_.clear();
    function_header_.clear();
    locals_.clear();
    instructions_.clear();
    captures_.clear();
    next_temporary_ = 0;
    in_function_ = false;
}

void WatWriter::module_item(std::string_view wat) {
    items_.emplace_back(wat);
}

void WatWriter::begin_function(std::string_view header) {
    if (in_function_) {
        throw std::logic_error("Nested WAT function");
    }
    in_function_ = true;
    function_header_ = header;
    locals_.clear();
    instructions_.clear();
    captures_.clear();
}

void WatWriter::add_local(std::string_view name, std::string_view type) {
    local_declaration("(local " + std::string(name) + " " + std::string(type) + ")");
}

void WatWriter::local_declaration(std::string_view wat) {
    if (!in_function_) {
        throw std::logic_error("WAT local outside function");
    }
    locals_.emplace_back(wat);
}

std::string WatWriter::temporary(std::string_view type) {
    const auto name = "$writer_tmp" + std::to_string(next_temporary_++);
    add_local(name, type);
    return name;
}

void WatWriter::instruction(std::string_view wat) {
    if (!in_function_) {
        throw std::logic_error("WAT instruction outside function");
    }
    auto& destination = captures_.empty() ? instructions_ : captures_.back();
    destination.emplace_back(wat);
}

std::string WatWriter::capture_instructions(const std::function<void()>& emit) {
    if (!in_function_) {
        throw std::logic_error("WAT capture outside function");
    }
    captures_.emplace_back();
    try {
        emit();
    } catch (...) {
        captures_.pop_back();
        throw;
    }

    auto captured = std::move(captures_.back());
    captures_.pop_back();
    std::ostringstream output;
    for (std::size_t index = 0; index < captured.size(); ++index) {
        if (index != 0) {
            output << '\n';
        }
        output << captured[index];
    }
    return output.str();
}

void WatWriter::end_function() {
    if (!in_function_ || !captures_.empty()) {
        throw std::logic_error("Invalid WAT function end");
    }
    std::ostringstream output;
    output << "(func " << function_header_ << '\n';
    for (const auto& local : locals_) {
        output << "  " << local << '\n';
    }
    for (const auto& instruction : instructions_) {
        output << indent(instruction, 1);
    }
    output << ')';
    items_.push_back(output.str());
    in_function_ = false;
}

std::string WatWriter::finish_module() const {
    if (in_function_) {
        throw std::logic_error("Unfinished WAT function");
    }
    std::ostringstream output;
    output << "(module\n";
    for (const auto& item : items_) {
        output << indent(item, 1);
    }
    output << ')';
    return output.str();
}
