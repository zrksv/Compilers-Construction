#include <iostream>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <filesystem>

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

void test_missing_routine_name() {
    int exit_code = run_source("no_routine_name.imp", "routine main() is print 1 end\n", "");
    assert(exit_code != 0 && "Compiler should fail when routine name is not passed");
    std::cout << "[PASS] App handles missing routine name\n";
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
    test_missing_routine_name();
    test_file_not_found();
    test_valid_routine();
    test_routine_not_found();
    test_wrong_argument_count();
    test_forward_only_routine();
    test_empty_file();
    test_syntax_error_file();

    std::cout << "\nAll App CLI tests passed successfully!\n";
    return 0;
}
