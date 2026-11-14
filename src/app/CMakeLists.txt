add_executable(fair_compiler main.cpp
        WasmExecution.cpp
        WasmExecution.hpp)
target_link_libraries(fair_compiler PRIVATE parser semantics optimizer)

add_subdirectory(tests)
