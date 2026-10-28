find_package(BISON 3.8 QUIET)

if(BISON_FOUND)
    set(PARSER_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
    file(MAKE_DIRECTORY ${PARSER_GEN_DIR})
    BISON_TARGET(ImperativeParser Parser.yy ${PARSER_GEN_DIR}/Parser.cpp
        DEFINES_FILE ${PARSER_GEN_DIR}/Parser.hpp)
    set(PARSER_GEN_SOURCES ${BISON_ImperativeParser_OUTPUTS})

    add_custom_target(update_parser_sources
        COMMAND ${BISON_EXECUTABLE} -o src/parser/Parser.cpp src/parser/Parser.yy
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Regenerating src/parser/Parser.cpp and Parser.hpp"
        VERBATIM)
else()
    message(STATUS "Bison >= 3.8 not found; using pre-generated src/parser/Parser.cpp")
    set(PARSER_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(PARSER_GEN_SOURCES ${PARSER_GEN_DIR}/Parser.cpp)
endif()

add_library(parser STATIC
    ParserDriver.cpp
    ${PARSER_GEN_SOURCES}
)
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(parser BEFORE PRIVATE ${PARSER_GEN_DIR})
target_link_libraries(parser PUBLIC lexer ast common)

add_subdirectory(tests)
