add_library(semantics STATIC
        types/Type.hpp
        types/Type.cpp
        types/TypeFactory.hpp
        types/TypeCompatibility.hpp
        types/TypeCompatibility.cpp
        symbols/Symbol.hpp
        symbols/Scope.hpp
        symbols/Scope.cpp
        SemanticModel.hpp
        SemanticModel.cpp
        Diagnostics.hpp
        Diagnostics.cpp
        Results.hpp
        Context.hpp
        Services.hpp
        Dispatcher.hpp
        RuleBase.hpp
        rules/common/CallChecker.hpp
        rules/common/CallChecker.cpp
        rules/types/ArrayTypeRule.hpp
        rules/types/IdentifierTypeRule.hpp
        rules/types/PrimitiveTypeRule.hpp
        rules/types/RecordTypeRule.hpp
        rules/expressions/LiteralRules.hpp
        rules/expressions/IdentifierRule.hpp
        rules/expressions/UnaryRule.hpp
        rules/expressions/BinaryRule.hpp
        rules/expressions/FieldAccessRule.hpp
        rules/expressions/ElementAccessRule.hpp
        rules/expressions/RoutineCallExprRule.hpp
        rules/declarations/ProgramRule.hpp
        rules/declarations/VariableDeclarationRule.hpp
        rules/declarations/TypeDeclarationRule.hpp
        rules/declarations/RoutineDeclarationRule.hpp
)

target_include_directories(semantics PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(semantics PUBLIC common ast)

if(MSVC)
    target_compile_options(semantics PRIVATE /EHsc /Zc:__cplusplus)
endif()

add_subdirectory(tests)
