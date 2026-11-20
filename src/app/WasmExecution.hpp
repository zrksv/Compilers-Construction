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

// The runner provides host.report_trap(row: i32, column: i32, kind: i32) -> void.
// Record the uint32 coordinates (JS: row >>> 0, column >>> 0), then use them in
// diagnostic on failure. Clear the saved report before instantiation and each call;
// a start-function failure can report a position before an instance is returned.
// The reporter must not write to the source program's stdout.
WasmExecutionResult execute_wasm(std::string_view wat, const RoutineSymbol& routine,
    const std::vector<std::string>& arguments);
