cmake_minimum_required(VERSION 3.29)
project(Compilers_Construction)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

function(add_imp_test)
    cmake_parse_arguments(TEST "" "TARGET;NAME" "SOURCES;LIBS" ${ARGN})
    add_executable(${TEST_TARGET} ${TEST_SOURCES})
    target_link_libraries(${TEST_TARGET} PRIVATE ${TEST_LIBS})
    target_compile_options(${TEST_TARGET} PRIVATE $<IF:$<CXX_COMPILER_ID:MSVC>,/UNDEBUG,-UNDEBUG>)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_TARGET})
endfunction()

add_subdirectory(src/common)
add_subdirectory(src/lexer)
add_subdirectory(src/ast)
add_subdirectory(src/parser)
add_subdirectory(src/semantics)
add_subdirectory(src/app)

add_subdirectory(tests)
