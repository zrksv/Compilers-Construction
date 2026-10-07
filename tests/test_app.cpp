#include <iostream>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <filesystem>

std::string get_exec_path() {
#ifdef _WIN32
    return "Compilers_Construction.exe";
#else
    return "./Compilers_Construction";
#endif
}

int run_app(const std::string& args) {
    std::string command = get_exec_path() + " " + args;
    return std::system(command.c_str());
}

void test_missing_arguments() {
    int exit_code = run_app("");
    assert(exit_code != 0 && "Compiler should fail when no arguments are passed");
    std::cout << "[PASS] App handles missing arguments\n";
}

void test_file_not_found() {
    int exit_code = run_app("non_existent_file_12345.imp");
    assert(exit_code != 0 && "Compiler should fail when file does not exist");
    std::cout << "[PASS] App handles non-existent file\n";
}

void test_tokens_flag() {
    std::ofstream("dummy_valid.imp") << "var x : integer is 5;";

    int exit_code = run_app("dummy_valid.imp --tokens");
    assert(exit_code == 0 && "Compiler should succeed in --tokens mode");

    std::filesystem::remove("dummy_valid.imp");
    std::cout << "[PASS] App handles --tokens flag\n";
}

void test_empty_file() {
    std::ofstream("empty.imp") << "";

    int exit_code = run_app("empty.imp");
    assert(exit_code == 0 && "Compiler should successfully parse an empty file!");

    std::filesystem::remove("empty.imp");
    std::cout << "[PASS] App successfully handles an empty file\n";
}

void test_syntax_error_file() {
    std::ofstream("syntax_error.imp") << "routine main() is var x : integer is @; ";

    int exit_code = run_app("syntax_error.imp");
    assert(exit_code != 0 && "Compiler MUST return non-zero exit code on syntax errors");

    std::filesystem::remove("syntax_error.imp");
    std::cout << "[PASS] App handles syntax errors gracefully and returns error code\n";
}

int main() {
    std::cout << "Running App CLI Tests...\n";

    test_missing_arguments();
    test_file_not_found();
    test_tokens_flag();
    test_empty_file();
    test_syntax_error_file();

    std::cout << "\nAll App CLI tests passed successfully!\n";
    return 0;
}