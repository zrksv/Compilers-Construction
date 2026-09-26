#include <iostream>
#include <cstdlib>
#include <cassert>

int run_app(const std::string& args) {
    std::string command = "./Compilers_Construction " + args;
    return std::system(command.c_str());
}

void test_missing_arguments() {
    int exit_code = run_app("");
    assert(exit_code != 0);
    std::cout << "[PASS] App handles missing arguments\n";
}

void test_file_not_found() {
    int exit_code = run_app("fake_directory/non_existent.imp");
    assert(exit_code != 0);
    std::cout << "[PASS] App handles non-existent file\n";
}

void test_syntax_error_file() {
    int exit_code = run_app("tests/samples/syntax_error.imp");
    assert(exit_code != 0);
    std::cout << "[PASS] App handles syntax errors gracefully\n";
}

void test_tokens_flag() {
    int exit_code = run_app("tests/samples/01_print.imp --tokens");
    assert(exit_code == 0); // Тут всё должно быть успешно (0)
    std::cout << "[PASS] App handles --tokens flag\n";
}

int main() {
    std::cout << "Running App CLI Tests...\n";
    test_missing_arguments();
    test_file_not_found();
    test_tokens_flag();
    // test_syntax_error_file(); // Раскомментируем, когда Лёня допишет обработку ошибок

    std::cout << "All App CLI tests passed!\n";
    return 0;
}