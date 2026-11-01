#include "rules/common/CallChecker.hpp"
#include "Dispatcher.hpp"
#include "Services.hpp"
#include "Diagnostics.hpp"
#include "SemanticModel.hpp"
#include "symbols/Scope.hpp"
#include "types/TypeCompatibility.hpp"

CallChecker::CallChecker(Dispatcher& dispatcher, Services& services)
    : m_dispatcher(dispatcher), m_services(services) {}

const RoutineSymbol* CallChecker::check(const AstNode* call,
                                       const std::string& name,
                                       const std::vector<std::unique_ptr<Expression>>& arguments,
                                       const Context& ctx) {
    std::vector<TypeRef> arg_types;
    arg_types.reserve(arguments.size());
    for (const auto& arg : arguments) {
        ExprResult res = m_dispatcher.expression(arg.get(), ctx);
        arg_types.push_back(res.type);
    }

    if (!ctx.scope) {
        return nullptr;
    }

    auto* routine = ctx.scope->lookup_routine(name);
    Location loc = call ? call->location : Location{};
    if (!routine) {
        m_services.diagnostics.report(loc, "Routine '" + name + "' is not declared");
        return nullptr;
    }

    if (call) {
        m_services.model.bind_routine(call, routine);
    }

    if (arguments.size() != routine->parameters.size()) {
        m_services.diagnostics.report(loc, "Routine '" + name + "' expects " +
                                           std::to_string(routine->parameters.size()) + " argument(s), got " +
                                           std::to_string(arguments.size()));
        return routine;
    }

    for (size_t i = 0; i < arguments.size(); ++i) {
        const auto& arg_type = arg_types[i];
        const auto& param_type = routine->parameters[i].type;

        if (!arg_type || arg_type->is_error() || !param_type || param_type->is_error()) {
            continue;
        }

        if (!TypeCompatibility::can_assign(param_type, arg_type)) {
            Location arg_loc = arguments[i] ? arguments[i]->location : loc;
            m_services.diagnostics.report(arg_loc,
                                          "Argument " + std::to_string(i + 1) + " expects type " +
                                          param_type->to_string() + ", got " + arg_type->to_string());
        } else if (arguments[i]) {
            ConversionKind kind = TypeCompatibility::conversion(param_type, arg_type);
            m_services.model.set_conversion(arguments[i].get(), kind);
        }
    }

    return routine;
}
