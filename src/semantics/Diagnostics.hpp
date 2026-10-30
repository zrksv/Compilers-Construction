#pragma once
#include <ostream>
#include <string>
#include <vector>
#include "../common/Location.hpp"
#include "DeclarationAnalyzer.hpp"

class Diagnostics {
private:
    std::vector<SemanticError> m_errors;

public:
    void report(const Location& loc, const std::string& message);
    void append(const std::vector<SemanticError>& errors);
    void sort();
    void print(std::ostream& out) const;

    bool has_errors() const {
        return !m_errors.empty();
    }

    const std::vector<SemanticError>& get_errors() const {
        return m_errors;
    }
};
