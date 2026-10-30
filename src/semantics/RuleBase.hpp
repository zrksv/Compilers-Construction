#pragma once
#include "Dispatcher.hpp"
#include "Services.hpp"
#include "Diagnostics.hpp"
#include "SemanticModel.hpp"
#include "types/TypeCompatibility.hpp"

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

    bool convert(const TypeRef& target, const TypeRef& source, Expression* value_expr = nullptr, const Location& loc = {}) {
        if (!target || !source) {
            return false;
        }
        if (target->is_error() || source->is_error()) {
            return true;
        }
        if (!TypeCompatibility::can_assign(target, source)) {
            Location err_loc = value_expr ? value_expr->location : loc;
            error(err_loc, "Cannot assign value of type " + source->to_string() + " to " + target->to_string());
            return false;
        }
        if (value_expr) {
            ConversionKind kind = TypeCompatibility::conversion(target, source);
            m_services.model.set_conversion(value_expr, kind);
        }
        return true;
    }
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
