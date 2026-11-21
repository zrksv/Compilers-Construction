#include <iostream>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <sstream>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <sys/stat.h>
#endif

std::filesystem::path work_file(const std::string& name) {
    return std::filesystem::temp_directory_path() / name;
}

std::string quote(const std::string& text) {
    return "\"" + text + "\"";
}

int run_app(const std::string& args) {
    std::string command = quote(IMP_COMPILER_PATH) + " " + args;
#ifdef _WIN32
    command = quote(command);
#endif
    return std::system(command.c_str());
}

int run_source(const std::string& name, const std::string& code, const std::string& args) {
    auto path = work_file(name);
    std::ofstream(path) << code;
    int exit_code = run_app(quote(path.string()) + " " + args);
    std::filesystem::remove(path);
    return exit_code;
}

void test_missing_arguments() {
    int exit_code = run_app("");
    assert(exit_code != 0 && "Compiler should fail when no arguments are passed");
    std::cout << "[PASS] App handles missing arguments\n";
}

// Omitting the routine name selects main with no arguments.
void test_default_main() {
    int exit_code = run_source("no_routine_name.imp", "routine main() is print 1 end\n", "");
    assert(exit_code == 0 && "Compiler should select main when the routine name is omitted");
}

// The default name still requires a main definition in the file.
void test_missing_default_main() {
    assert(run_source("missing_main.imp", "routine other() is end\n", "") != 0);
}

// Parameters for main require its explicit name.
void test_main_with_explicit_arguments() {
    const std::string code = "routine main(value : integer) is end\n";
    assert(run_source("main_args.imp", code, "main 7") == 0);
    assert(run_source("main_missing_args.imp", code, "") != 0);
    assert(run_source("main_no_name.imp", code, "7") != 0);
}

// A forward declaration does not hide the complete definition chosen for validation.
void test_forward_and_definition() {
    assert(run_source("forward_definition.imp", "routine f(x : integer) : integer\n"
        "routine f(value : integer) : integer => value\n", "f 7") == 0);
}

// Signed i32 boundaries are valid while out-of-range integer literals are rejected.
void test_integer_argument_boundaries() {
    const std::string code = "routine f(value : integer) is end\n";
    assert(run_source("arg_min.imp", code, "f -2147483648") == 0);
    assert(run_source("arg_max.imp", code, "f +2147483647") == 0);
    assert(run_source("arg_overflow.imp", code, "f 2147483648") != 0);
    assert(run_source("arg_underflow.imp", code, "f -2147483649") != 0);
}

// Validation follows primitive parameter conversions, including aliases and rounding.
void test_primitive_argument_conversions() {
    const std::string code = "type Count is integer\nroutine f(i : Count, r : real, b : boolean) is end\n";
    assert(run_source("arg_conversions.imp", code, "f 2.5 true 1") == 0);
    assert(run_source("arg_bool_to_int.imp", code, "f false +3.0 true") == 0);
    assert(run_source("arg_negative_real.imp", code, "f -2.5 -3. false") == 0);
    assert(run_source("arg_round_overflow.imp", code, "f 2147483647.5 0 false") != 0);
}

// Boolean parameters accept only booleans or integer 0/1, never real values.
void test_boolean_argument_validation() {
    const std::string code = "routine f(value : boolean) is end\n";
    assert(run_source("arg_bool_zero.imp", code, "f 0") == 0);
    assert(run_source("arg_bool_two.imp", code, "f 2") != 0);
    assert(run_source("arg_bool_real.imp", code, "f 1.0") != 0);
    assert(run_source("arg_bool_text.imp", code, "f truthy") != 0);
}

// Numeric tokens must be whole literals rather than valid numeric prefixes or expressions.
void test_malformed_argument_values() {
    const std::string code = "routine f(value : real) is end\n";
    assert(run_source("arg_suffix.imp", code, "f 12abc") != 0);
    assert(run_source("arg_expression.imp", code, "f 1+2") != 0);
    assert(run_source("arg_double_sign.imp", code, "f +-1") != 0);
}

// Real parsing accepts a decimal point and rejects values larger than f64 can hold.
void test_real_argument_parsing() {
    const std::string code = "routine f(value : real) is end\n";
    assert(run_source("arg_real_fraction.imp", code, "f -2.75") == 0);
    assert(run_source("arg_real_point.imp", code, "f 3.") == 0);
    assert(run_source("arg_real_comma.imp", code, "f 2,75") != 0);
    assert(run_source("arg_real_overflow.imp", code, "f " + std::string(400, '9') + ".0") != 0);
}

// Nonzero real literals rounded to zero are rejected consistently on MSVC and MinGW.
void test_real_argument_underflow() {
    const std::string code = "routine f(value : real) is end\n";
    const std::string tiny = "0." + std::string(400, '0') + "1";
    assert(run_source("arg_real_underflow.imp", code, "f " + tiny) != 0);
    assert(run_source("arg_real_negative_underflow.imp", code, "f -" + tiny) != 0);
    const std::string below_half = "0." + std::string(323, '0') + "2";
    assert(run_source("arg_real_rounds_to_zero.imp", code, "f " + below_half) != 0);
}

// Genuine signed zero and representable subnormal values remain valid.
void test_real_argument_zero_and_subnormal() {
    const std::string code = "routine f(value : real) is end\n";
    const std::string zero = "0." + std::string(400, '0');
    const std::string subnormal = "0." + std::string(323, '0') + "5";
    assert(run_source("arg_real_long_zero.imp", code, "f " + zero) == 0);
    assert(run_source("arg_real_negative_zero.imp", code, "f -" + zero) == 0);
    assert(run_source("arg_real_subnormal.imp", code, "f " + subnormal) == 0);
    assert(run_source("arg_real_negative_subnormal.imp", code, "f -" + subnormal) == 0);
}

// An exported routine with aggregate parameters cannot be selected from the CLI.
void test_aggregate_entry_parameters() {
    assert(run_source("arg_array.imp", "routine f(values : array [] integer) is end\n", "f 0") != 0);
    assert(run_source("arg_record.imp", "type R is record end\nroutine f(value : R) is end\n", "f 0") != 0);
}

struct ConsoleResult { int exit_code; std::string output; };

ConsoleResult run_console(const std::string& name, const std::string& input) {
    const auto input_path = work_file(name + ".in");
    const auto output_path = work_file(name + ".out");
    std::ofstream(input_path) << input;
#ifdef _WIN32
    // Pass redirected CRT descriptors directly to the child, without a cmd.exe shell.
    std::cout.flush();
    std::cerr.flush();
    const int saved_input = _dup(0), saved_output = _dup(1), saved_error = _dup(2);
    const int input_fd = _open(input_path.string().c_str(), _O_RDONLY | _O_TEXT);
    const int output_fd = _open(output_path.string().c_str(), _O_WRONLY | _O_CREAT | _O_TRUNC | _O_TEXT, _S_IREAD | _S_IWRITE);
    assert(input_fd >= 0 && output_fd >= 0 && saved_input >= 0 && saved_output >= 0 && saved_error >= 0);
    _dup2(input_fd, 0);
    _dup2(output_fd, 1);
    _dup2(output_fd, 2);
    _close(input_fd);
    _close(output_fd);
    const int exit_code = static_cast<int>(_spawnl(_P_WAIT, IMP_COMPILER_PATH, IMP_COMPILER_PATH, "--interactive", nullptr));
    _dup2(saved_input, 0);
    _dup2(saved_output, 1);
    _dup2(saved_error, 2);
    _close(saved_input);
    _close(saved_output);
    _close(saved_error);
#else
    const std::string command = quote(IMP_COMPILER_PATH) + " --interactive < " + quote(input_path.string())
        + " > " + quote(output_path.string()) + " 2>&1";
    const int exit_code = std::system(command.c_str());
#endif
    std::ifstream output_file(output_path);
    std::ostringstream captured;
    captured << output_file.rdbuf();
    if (exit_code != 0) { std::cerr << "Console exit code: " << exit_code << '\n' << captured.str(); }
    output_file.close();
    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
    return {exit_code, captured.str()};
}

// Interactive commands execute routines and print their language output.
void test_interactive_execution() {
    const auto path = work_file("interactive program.imp");
    std::ofstream(path) << "routine add(a : integer, b : integer) is print a + b end\n";
    const auto file = quote(path.string());
    const auto result = run_console("interactive_validation", file + " add 1 2\n"
        + file + " add bad 2\n" + file + " add 3 4\n:quit\n");
    std::filesystem::remove(path);
    assert(result.exit_code == 0);
    assert(result.output.find("> 3\n") != std::string::npos);
    assert(result.output.find("> 7\n") != std::string::npos);
    assert(result.output.find("argument 1") != std::string::npos);
}

// A missing file reports an error and EOF exits the console cleanly.
void test_interactive_without_file() {
    const auto result = run_console("interactive_no_file", "non_existent_console_file.imp\n");
    assert(result.exit_code == 0 && result.output.find("Cannot open file") != std::string::npos);
}

// Calls from different files do not share a selected file, including after a failed command.
void test_interactive_multiple_files() {
    const auto first = work_file("interactive_first.imp");
    const auto second = work_file("interactive_second.imp");
    std::ofstream(first) << "routine first() is print 1 end\n";
    std::ofstream(second) << "routine second() is print 2 end\n";
    const auto result = run_console("interactive_replace", quote(first.string()) + " first\n"
        + "non_existent_console_file.imp\n" + quote(second.string()) + " second\n"
        + quote(first.string()) + " second\n:quit\n");
    std::filesystem::remove(first);
    std::filesystem::remove(second);
    assert(result.exit_code == 0);
    assert(result.output.find("> 1\n") != std::string::npos);
    assert(result.output.find("> 2\n") != std::string::npos);
    assert(result.output.find("routine 'second' not found") != std::string::npos);
}

// File-only commands select main, and a malformed path does not end the session.
void test_interactive_default_main() {
    const auto path = work_file("interactive default.imp");
    std::ofstream(path) << "routine main() is print 42 end\nroutine f(value : integer) is print value end\n";
    const auto result = run_console("interactive_default", "\n\"unclosed path\n" + quote(path.string())
        + "\n" + quote(path.string()) + " f 7\n:quit\n");
    std::filesystem::remove(path);
    assert(result.exit_code == 0);
    assert(result.output.find("invalid quoted file path") != std::string::npos);
    assert(result.output.find("> 42\n") != std::string::npos);
    assert(result.output.find("> 7\n") != std::string::npos);
}

// Quotes group the file path only; quoted parameter tokens are invalid in the console.
void test_interactive_parameter_quotes() {
    const auto path = work_file("interactive parameter quotes.imp");
    std::ofstream(path) << "routine f(value : integer) is print value end\n";
    const auto file = quote(path.string());
    const auto result = run_console("interactive_quotes", file + " f \"7\"\n" + file + " f 7\n:quit\n");
    std::filesystem::remove(path);
    assert(result.exit_code == 0);
    assert(result.output.find("argument 1") != std::string::npos);
    assert(result.output.find("> 7\n") != std::string::npos);
}

void test_file_not_found() {
    int exit_code = run_app("non_existent_file_12345.imp main");
    assert(exit_code != 0 && "Compiler should fail when file does not exist");
    std::cout << "[PASS] App handles non-existent file\n";
}

void test_valid_routine() {
    int exit_code = run_source("valid.imp",
                               "routine add(a : integer, b : integer) : integer => a + b\n"
                               "routine main() is print add(1, 2) end\n",
                               "add 1 2");
    assert(exit_code == 0 && "Compiler should succeed for an existing routine with matching arguments");
    std::cout << "[PASS] App accepts an existing routine with matching arguments\n";
}

void test_routine_not_found() {
    int exit_code = run_source("not_found.imp", "routine main() is print 1 end\n", "start");
    assert(exit_code != 0 && "Compiler should fail when routine does not exist");
    std::cout << "[PASS] App handles unknown routine\n";
}

void test_wrong_argument_count() {
    int exit_code = run_source("wrong_args.imp", "routine f(x : integer) is print x end\n", "f");
    assert(exit_code != 0 && "Compiler should fail when argument count does not match");
    std::cout << "[PASS] App handles wrong argument count\n";
}

void test_forward_only_routine() {
    int exit_code = run_source("forward_only.imp", "routine f(x : integer)\n", "f 1");
    assert(exit_code != 0 && "Compiler should fail for a forward-only routine");
    std::cout << "[PASS] App handles forward-only routine\n";
}

void test_empty_file() {
    int exit_code = run_source("empty.imp", "", "main");
    assert(exit_code != 0 && "An empty file has no routine to run");
    std::cout << "[PASS] App handles an empty file\n";
}

void test_syntax_error_file() {
    int exit_code = run_source("syntax_error.imp", "routine main() is var x : integer is @; ", "main");
    assert(exit_code != 0 && "Compiler MUST return non-zero exit code on syntax errors");
    std::cout << "[PASS] App handles syntax errors gracefully and returns error code\n";
}

int main() {
    std::cout << "Running App CLI Tests...\n";

    test_missing_arguments();
    test_missing_default_main();
#ifdef IMP_APP_RUNTIME_AVAILABLE
    test_default_main();
    test_main_with_explicit_arguments();
    test_forward_and_definition();
    test_integer_argument_boundaries();
    test_primitive_argument_conversions();
    test_boolean_argument_validation();
    test_malformed_argument_values();
    test_real_argument_parsing();
    test_real_argument_underflow();
    test_real_argument_zero_and_subnormal();
#endif
    test_aggregate_entry_parameters();
    test_interactive_without_file();
#ifdef IMP_APP_RUNTIME_AVAILABLE
    test_interactive_execution();
    test_interactive_multiple_files();
    test_interactive_default_main();
    test_interactive_parameter_quotes();
#else
    std::cout << "[SKIP] Interactive execution tests require wat2wasm, wasm-validate, and Node.js\n";
#endif
    test_file_not_found();
#ifdef IMP_APP_RUNTIME_AVAILABLE
    test_valid_routine();
#else
    std::cout << "[SKIP] App execution test requires wat2wasm, wasm-validate, and Node.js\n";
#endif
    test_routine_not_found();
    test_wrong_argument_count();
    test_forward_only_routine();
    test_empty_file();
    test_syntax_error_file();

    std::cout << "\nAll App CLI tests passed successfully!\n";
    return 0;
}
