cmake_minimum_required(VERSION 3.29)
project(Compilers_Construction)

set(CMAKE_CXX_STANDARD 20)
include_directories(src)

add_executable(Compilers_Construction
        src/app/main.cpp
        src/lexer/Token.cpp
        src/lexer/Token.hpp
        src/common/SourceManager.cpp
        src/common/SourceManager.hpp
        src/lexer/Lexer.cpp
        src/lexer/Lexer.hpp
        src/common/Location.hpp
        src/ast/AST.hpp)

add_executable(lexer_tests
    tests/test_lexer.cpp
        src/lexer/Token.cpp
        src/common/SourceManager.cpp
        src/lexer/Lexer.cpp
)
