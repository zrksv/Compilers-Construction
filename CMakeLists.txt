cmake_minimum_required(VERSION 3.29)
project(Compilers_Construction)

set(CMAKE_CXX_STANDARD 20)
include_directories(src)
include_directories(src/common)
include_directories(src/lexer)
include_directories(src/ast)
include_directories(src/parser)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(BISON REQUIRED)

BISON_TARGET(MyParser src/parser/Parser.yy ${CMAKE_CURRENT_BINARY_DIR}/Parser.cpp)

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
        src/parser/ParserDriver.hpp
        src/parser/ParserDriver.cpp
        ${BISON_MyParser_OUTPUTS}
)

add_executable(lexer_tests
        src/lexer/tests/test_lexer.cpp
        tests/TestUtils.cpp
        src/lexer/Token.cpp
        src/common/SourceManager.cpp
        src/lexer/Lexer.cpp
)

enable_testing()

add_subdirectory(src/common)
add_subdirectory(src/lexer)
add_subdirectory(src/ast)
add_subdirectory(src/parser)
add_subdirectory(src/app)

add_subdirectory(tests)