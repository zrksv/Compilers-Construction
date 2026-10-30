#include "Diagnostics.hpp"
#include <algorithm>

void Diagnostics::report(const Location& loc, const std::string& message) {
    m_errors.push_back({loc, message});
}

void Diagnostics::append(const std::vector<SemanticError>& errors) {
    m_errors.insert(m_errors.end(), errors.begin(), errors.end());
}

void Diagnostics::sort() {
    std::stable_sort(m_errors.begin(), m_errors.end(), [](const SemanticError& a, const SemanticError& b) {
        if (a.location.row != b.location.row) {
            return a.location.row < b.location.row;
        }
        return a.location.column < b.location.column;
    });
}

void Diagnostics::print(std::ostream& out) const {
    for (const auto& err : m_errors) {
        out << "Error at " << err.location.row << ":" << err.location.column
            << " - " << err.message << "\n";
    }
}
