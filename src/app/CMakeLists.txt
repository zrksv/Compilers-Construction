add_executable(fair_compiler main.cpp)

target_link_libraries(fair_compiler PRIVATE parser lexer ast common)