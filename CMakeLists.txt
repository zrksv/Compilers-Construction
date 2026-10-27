cmake_minimum_required(VERSION 3.29)
project(Compilers_Construction)

set(CMAKE_CXX_STANDARD 20)
add_compile_definitions(PROJECT_BUILD_DIR="${CMAKE_BINARY_DIR}")
include_directories(src)
include_directories(src/common)
include_directories(src/lexer)
include_directories(src/ast)
include_directories(src/parser)

add_executable(Compilers_Construction
        src/app/main.cpp
        src/lexer/Token.cpp
        src/lexer/Token.hpp
        src/common/SourceManager.cpp
        src/common/SourceManager.hpp
        src/lexer/Lexer.cpp
        src/lexer/Lexer.hpp
        src/common/Location.hpp
        src/ast/AST.hpp
        src/ast/AST.cpp
        src/ast/ASTPrinter.hpp
        src/ast/ASTPrinter.cpp
)
target_link_libraries(Compilers_Construction PRIVATE parser)

enable_testing()

add_subdirectory(src/common)
add_subdirectory(src/lexer)
add_subdirectory(src/ast)
add_subdirectory(src/parser)
add_subdirectory(src/app)

add_subdirectory(tests)