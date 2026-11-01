#pragma once
#include "Dispatcher.hpp"
#include "Services.hpp"
#include "Diagnostics.hpp"
#include "SemanticModel.hpp"
#include "types/TypeFactory.hpp"
#include "types/TypeCompatibility.hpp"

// Base class providing common infrastructure to all semantic rules
class RuleBase {
protected:
    Dispatcher& m_dispatcher;
    Services& m_services;

public:
    RuleBase(Dispatcher& dispatcher, Services& services)
        : m_dispatcher(dispatcher), m_services(services) {}

    virtual ~RuleBase() = default;

    void error(const Location& loc, const std::string& message) {
        m_services.diagnostics.report(loc, message);
    }

    bool convert(const TypeRef& target, const TypeRef& source, Expression* value_expr = nullptr, const Location& loc = {});
};

template<typename NodeT>
class ExpressionRule : public RuleBase {
public:
    using RuleBase::RuleBase;
    virtual ExprResult analyze(NodeT& node, const Context& ctx) = 0;
};

template<typename NodeT>
class TypeRule : public RuleBase {
public:
    using RuleBase::RuleBase;
    virtual TypeRef resolve(NodeT& node, const Context& ctx) = 0;
};

template<typename NodeT>
class StatementRule : public RuleBase {
public:
    using RuleBase::RuleBase;
    virtual StmtResult analyze(NodeT& node, const Context& ctx) = 0;
};
