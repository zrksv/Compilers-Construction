cmake_minimum_required(VERSION 3.29)
project(Compilers_Construction)

set(CMAKE_CXX_STANDARD 20)

add_executable(Compilers_Construction
    src/main.cpp
        src/Token.cpp
        src/Token.hpp
        src/SourceManager.cpp
        src/SourceManager.hpp
        src/Lexer.cpp
        src/Lexer.hpp
        src/Location.hpp)
