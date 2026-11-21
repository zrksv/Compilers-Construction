#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "SourceManager.hpp"
#include "Lexer.hpp"
#include "ParserDriver.hpp"
#include "SemanticAnalyzer.hpp"
#include "WasmCodegen.hpp"
#include "WasmExecution.hpp"
#include "optimizer/AstOptimizer.hpp"

namespace {
struct LoadedProgram {
    ParserDriver parser;
    SemanticAnalyzer analyzer;
    Program* program = nullptr;
};

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

int invoke_file(const std::string& filename, const std::string& name,
                const std::vector<std::string>& arguments, bool optimize = true) {
    auto loaded = load_program(filename);
    if (!loaded) { return 1; }

    auto& model = loaded->analyzer.get_model();
    const auto* routine = model.global_scope().lookup_routine(name, false);
    if (!routine || !routine->is_defined) {
        std::cerr << "Error: routine '" << name << "' not found in '" << filename << "'.\n";
        return 1;
    }

    try {
        if (optimize) {
            AstOptimizer optimizer(model);
            optimizer.optimize(loaded->program);
        }
        WasmCodegen codegen;
        const auto wat = codegen.generate_wat(*loaded->program, model);
        const auto result = execute_wasm(wat, *routine, arguments);
        std::cout << result.stdout_text << std::flush;
        if (!result.diagnostic.empty()) {
            std::cerr << result.diagnostic;
            if (result.diagnostic.back() != '\n') { std::cerr << '\n'; }
        }
        return result.exit_code;
    } catch (const std::exception& error) {
        std::cerr << "Compilation or execution failed: " << error.what() << '\n';
        return 1;
    }
}

int interactive_console() {
    std::cout << "Enter <file.imp> [routine_name] [args...] to run a program, or :quit.\n"
                 "Omitting the routine name runs main. Quote file paths with spaces.\n";
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
        invoke_file(filename, routine, arguments);
    }
    return 0;
}

}

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string_view(argv[1]) == "--interactive") {
        return interactive_console();
    }

    const bool optimize = argc < 2 || std::string_view(argv[1]) != "--no-opt";
    const int first = optimize ? 1 : 2;
    if (argc <= first || std::string_view(argv[first]) == "--interactive") {
        std::cerr << "Usage: fair_compiler [--no-opt] <source.imp> [routine_name] [args...]\n"
                     "       fair_compiler --interactive\n";
        return 1;
    }

    std::vector<std::string> arguments;
    for (int index = first + 2; index < argc; ++index) { arguments.emplace_back(argv[index]); }
    const std::string routine = argc > first + 1 ? argv[first + 1] : "main";
    return invoke_file(argv[first], routine, arguments, optimize);
}
