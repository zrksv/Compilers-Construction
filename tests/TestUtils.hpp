#pragma once
#include <string>
#include <filesystem>
#include "../src/lexer/Token.hpp"

std::string escape_token_text(const std::string& text);
std::string read_file(const std::string& path);
std::filesystem::path find_tests_dir();
std::string resolve_path(const std::string& name);