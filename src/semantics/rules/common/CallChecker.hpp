#pragma once
#include <string>
#include <vector>
#include <memory>
#include "ast/AST.hpp"
#include "symbols/Symbol.hpp"
#include "Context.hpp"

class Dispatcher;
struct Services;

class CallChecker {
private:
    Dispatcher& m_dispatcher;
    Services& m_services;

public:
    CallChecker(Dispatcher& dispatcher, Services& services);

    const RoutineSymbol* check(const AstNode* call,
                               const std::string& name,
                               const std::vector<std::unique_ptr<Expression>>& arguments,
                               const Context& ctx);
};
