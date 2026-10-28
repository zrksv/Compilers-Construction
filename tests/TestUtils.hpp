#pragma once
#include <string>
#include <filesystem>
#include <source_location>

std::string escape_token_text(const std::string& text);
std::string read_file(const std::string& path);
std::filesystem::path find_tests_dir();
std::filesystem::path test_output_dir(std::source_location caller = std::source_location::current());
std::string resolve_path(const std::string& name);
