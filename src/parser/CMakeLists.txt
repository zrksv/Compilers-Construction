add_library(parser
    ParserDriver.cpp
    Parser.cpp
)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_subdirectory(tests)
