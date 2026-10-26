add_executable(lexer_tests
        ${CMAKE_SOURCE_DIR}/src/lexer/tests/test_lexer.cpp
        TestUtils.cpp
        ${CMAKE_SOURCE_DIR}/src/lexer/Token.cpp
        ${CMAKE_SOURCE_DIR}/src/common/SourceManager.cpp
        ${CMAKE_SOURCE_DIR}/src/lexer/Lexer.cpp
)
target_link_libraries(lexer_tests PRIVATE common lexer)

add_executable(test_pipeline test_pipeline.cpp TestUtils.cpp)
target_link_libraries(test_pipeline PRIVATE parser lexer ast common)

add_executable(test_app test_app.cpp)