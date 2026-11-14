#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

struct RoutineSymbol;

using WasmResult = std::variant<std::monostate, std::int32_t, double, bool, std::uint32_t>;

struct WasmExecutionResult {
    int exit_code;
    WasmResult value;
    std::string stdout_text;
    std::string diagnostic;
};

WasmExecutionResult execute_wasm(std::string_view wat, const RoutineSymbol& routine,
    const std::vector<std::string>& arguments);