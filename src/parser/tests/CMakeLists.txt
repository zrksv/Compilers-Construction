add_executable(test_parser
    test_parser.cpp
    ../../lexer/Token.cpp
    ../../common/SourceManager.cpp
    ../../lexer/Lexer.cpp
)

target_include_directories(test_parser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(test_parser PRIVATE parser ast)

add_test(NAME ParserTest COMMAND test_parser)
