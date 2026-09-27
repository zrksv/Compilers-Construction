#include "TestUtils.hpp"
#include <fstream>
#include <sstream>

std::string escape_token_text(const std::string& text) {
    std::string res;
    for (char c : text) {
        if (c == '\n') res += "\\n";
        else if (c == '\t') res += "\\t";
        else if (c == '\r') res += "\\r";
        else if (c == '\\') res += "\\\\";
        else res += c;
    }
    return res;
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::filesystem::path find_tests_dir() {
    std::filesystem::path cur = std::filesystem::current_path();
    for (int i = 0; i < 5; ++i) {
        if (std::filesystem::exists(cur / "tests" / "samples")) return cur / "tests";
        if (std::filesystem::exists(cur / "samples")) return cur;
        if (!cur.has_parent_path()) break;
        cur = cur.parent_path();
    }
    return "tests";
}

std::string resolve_path(const std::string& name) {
    std::string file = name.ends_with(".imp") ? name : name + ".imp";
    return (find_tests_dir() / "samples" / file).string();
}