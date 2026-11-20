#include <charconv>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <locale>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include "SourceManager.hpp"
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "SemanticAnalyzer.hpp"

namespace {
struct LoadedProgram {
    std::string filename;
    ParserDriver parser;
    SemanticAnalyzer analyzer;
    Program* program = nullptr;
};

enum class ArgumentKind { Integer, Real, Boolean };
struct ArgumentValue {
    ArgumentKind kind;
    double value;
};

bool parse_argument(std::string_view text, ArgumentValue& result) {
    if (text == "true" || text == "false") {
        result = {ArgumentKind::Boolean, text == "true" ? 1.0 : 0.0};
        return true;
    }
    if (!text.empty() && text.front() == '+') {
        text.remove_prefix(1);
        if (!text.empty() && text.front() == '-') { return false; }
    }
    if (text.empty()) { return false; }
    std::size_t position = text.front() == '-' ? 1 : 0;
    const auto first_digit = position;
    while (position < text.size() && text[position] >= '0' && text[position] <= '9') { ++position; }
    if (position == first_digit) { return false; }
    const bool real = position < text.size() && text[position] == '.';
    if (real) {
        ++position;
        while (position < text.size() && text[position] >= '0' && text[position] <= '9') { ++position; }
    }
    if (position != text.size()) { return false; }
    if (real) {
        double value = 0;
        std::istringstream input{std::string(text)};
        input.imbue(std::locale::classic());
        input >> std::noskipws >> value;
        if (!input || input.peek() != std::char_traits<char>::eof() || !std::isfinite(value)) { return false; }
        // Streams differ on underflow: reject a nonzero literal rounded to zero explicitly.
        if (value == 0 && text.find_first_of("123456789") != std::string_view::npos) { return false; }
        result = {ArgumentKind::Real, value};
    } else {
        std::int32_t value = 0;
        const auto parsed = std::from_chars(text.data(), text.data() + text.size(), value);
        if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size()) { return false; }
        result = {ArgumentKind::Integer, static_cast<double>(value)};
    }
    return true;
}

bool argument_fits(const ArgumentValue& argument, const Type& type) {
    if (type.is_real()) { return true; }
    if (type.is_boolean()) {
        return argument.kind != ArgumentKind::Real && (argument.value == 0 || argument.value == 1);
    }
    if (type.is_integer()) {
        const auto value = argument.kind == ArgumentKind::Real ? std::round(argument.value) : argument.value;
        return value >= std::numeric_limits<std::int32_t>::min() && value <= std::numeric_limits<std::int32_t>::max();
    }
    return false;
}

std::unique_ptr<LoadedProgram> load_program(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Cannot open file: " << filename << '\n';
        return nullptr;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    SourceManager source(buffer.str());
    Lexer lexer(source);
    auto loaded = std::make_unique<LoadedProgram>();
    loaded->filename = filename;
    if (!loaded->parser.parse(lexer)) {
        std::cerr << "Parsing failed due to syntax errors:\n";
        for (const auto& error : loaded->parser.get_errors()) {
            std::cerr << "Error at " << error.location.row << ':' << error.location.column << " - " << error.message << '\n';
        }
        return nullptr;
    }
    loaded->program = dynamic_cast<Program*>(loaded->parser.get_ast());
    if (!loaded->program || !loaded->analyzer.analyze(loaded->program)) {
        std::cerr << "Semantic analysis failed:\n";
        loaded->analyzer.get_diagnostics().print(std::cerr);
        return nullptr;
    }
    return loaded;
}

bool validate_invocation(LoadedProgram& loaded, const std::string& name, const std::vector<std::string>& arguments) {
    const RoutineSymbol* routine = nullptr;
    for (const auto& declaration : loaded.program->declarations) {
        const auto* candidate = dynamic_cast<const RoutineDeclaration*>(declaration.get());
        if (candidate && candidate->name == name && candidate->body_type != RoutineBodyType::Forward) {
            routine = loaded.analyzer.get_model().routine_of(candidate);
            break;
        }
    }
    if (!routine) {
        std::cerr << "Error: routine '" << name << "' not found in '" << loaded.filename << "'.\n";
        return false;
    }
    for (const auto& parameter : routine->parameters) {
        if (!parameter.type || (!parameter.type->is_integer() && !parameter.type->is_real() && !parameter.type->is_boolean())) {
            std::cerr << "Error: routine '" << name << "' has a non-primitive parameter '" << parameter.name << "'.\n";
            return false;
        }
    }
    if (arguments.size() != routine->parameters.size()) {
        std::cerr << "Error: routine '" << name << "' expects " << routine->parameters.size()
                  << " argument(s), but " << arguments.size() << " provided.\n";
        return false;
    }
    for (std::size_t i = 0; i < arguments.size(); ++i) {
        ArgumentValue argument{};
        const auto& parameter = routine->parameters[i];
        if (!parse_argument(arguments[i], argument) || !argument_fits(argument, *parameter.type)) {
            std::cerr << "Error: argument " << i + 1 << " ('" << arguments[i] << "') is invalid for parameter '"
                      << parameter.name << "' of type " << parameter.type->to_string() << ".\n";
            return false;
        }
    }
    std::cout << "Arguments valid for routine '" << name << "'.\n";
    return true;
}

int validate_file_invocation(const std::string& filename, const std::string& routine, const std::vector<std::string>& arguments) {
    auto loaded = load_program(filename);
    return loaded && validate_invocation(*loaded, routine, arguments) ? 0 : 1;
}

int interactive_console() {
    std::cout << "Argument validation console. Enter <file.imp> [routine_name] [args...], or :quit.\n"
                 "Quote file paths with spaces; enter parameters without quotes.\n";
    std::string line;
    while (std::cout << "fair> " << std::flush, std::getline(std::cin, line)) {
        std::istringstream command(line);
        command >> std::ws;
        if (command.peek() == std::char_traits<char>::eof()) { continue; }
        std::string filename;
        if (command.peek() == '"') {
            command.get();
            std::getline(command, filename, '"');
            if (command.eof() || filename.empty()) {
                std::cerr << "Error: invalid quoted file path.\n";
                continue;
            }
            const auto next = command.peek();
            if (next != std::char_traits<char>::eof() && next != ' ' && next != '\t' && next != '\r') {
                std::cerr << "Error: expected a space after the file path.\n";
                continue;
            }
        } else {
            command >> filename;
        }
        if (filename == ":quit") { return 0; }
        std::string routine;
        if (!(command >> routine)) { routine = "main"; }
        std::vector<std::string> arguments;
        for (std::string argument; command >> argument;) { arguments.push_back(std::move(argument)); }
        validate_file_invocation(filename, routine, arguments);
    }
    return 0;
}

}

int main(int argc, char* argv[]) {
    if (argc < 2 || (std::string_view(argv[1]) == "--interactive" && argc != 2)) {
        std::cerr << "Usage: fair_compiler <source.imp> [routine_name] [args...]\n"
                     "       fair_compiler --interactive\n";
        return 1;
    }
    if (std::string_view(argv[1]) == "--interactive") { return interactive_console(); }
    const std::string routine = argc > 2 ? argv[2] : "main";
    std::vector<std::string> arguments;
    for (int i = 3; i < argc; ++i) { arguments.emplace_back(argv[i]); }
    return validate_file_invocation(argv[1], routine, arguments);
}
