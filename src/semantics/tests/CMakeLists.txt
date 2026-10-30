add_imp_test(TARGET test_semantics NAME SemanticsTest SOURCES test_semantics.cpp LIBS semantics ast common test_utils)
if(MSVC)
    target_compile_options(test_semantics PRIVATE /EHsc /Zc:__cplusplus)
endif()

add_imp_test(TARGET test_statements NAME StatementsTest SOURCES test_statements.cpp LIBS semantics parser)
if(MSVC)
    target_compile_options(test_statements PRIVATE /EHsc /Zc:__cplusplus)
endif()
