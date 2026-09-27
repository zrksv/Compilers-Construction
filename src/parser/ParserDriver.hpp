#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "../common/Location.hpp"
#include "../common/SourceManager.hpp"
#include "../lexer/Lexer.hpp"
#include "../lexer/Token.hpp"
#include "../ast/AST.hpp"

struct ParseError {
    Location location;
    std::string message;
};

class ParserDriver {
private:
    Lexer* lexer = nullptr;
    std::unique_ptr<AstNode> ast_root;
    std::vector<ParseError> errors;
    bool trace_parsing = false;
    bool trace_scanning = false;

public:
    ParserDriver() = default;
    ~ParserDriver() = default;

    bool parse(Lexer& lexer);
    bool parse(SourceManager& source_manager);
    bool parse(const std::string& source_code);
    bool parse_file(const std::string& filename);

    AstNode* get_ast() const { return ast_root.get(); }
    std::unique_ptr<AstNode> take_ast() { return std::move(ast_root); }
    void set_ast_root(std::unique_ptr<AstNode> root) { ast_root = std::move(root); }

    void error(const Location& loc, const std::string& message) {
        errors.push_back({loc, message});
    }
    bool has_errors() const { return !errors.empty(); }
    const std::vector<ParseError>& get_errors() const { return errors; }
    void clear_errors() { errors.clear(); }

    Lexer* get_lexer() const { return lexer; }

    void set_trace_parsing(bool trace) { trace_parsing = trace; }
    bool get_trace_parsing() const { return trace_parsing; }
    void set_trace_scanning(bool trace) { trace_scanning = trace; }
    bool get_trace_scanning() const { return trace_scanning; }
};
