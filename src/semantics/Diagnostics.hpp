#pragma once
#include <ostream>
#include <string>
#include <vector>
#include "../common/Location.hpp"

struct SemanticError {
    Location location;
    std::string message;
};

class Diagnostics {
private:
    std::vector<SemanticError> m_errors;

public:
    void report(const Location& loc, const std::string& message);
    void append(const std::vector<SemanticError>& errors);
    void sort();
    void print(std::ostream& out) const;
    void clear();

    bool has_errors() const {
        return !m_errors.empty();
    }

    const std::vector<SemanticError>& get_errors() const {
        return m_errors;
    }
};
