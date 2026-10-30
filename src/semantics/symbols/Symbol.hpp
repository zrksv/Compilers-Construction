#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Type.hpp"
#include "../common/Location.hpp"

struct VariableSymbol {
    std::string name;
    std::shared_ptr<Type> type;
    bool is_readonly = false;
    Location location;

    VariableSymbol(std::string name,
                   std::shared_ptr<Type> type,
                   bool is_readonly = false,
                   Location loc = {})
        : name(std::move(name)),
          type(std::move(type)),
          is_readonly(is_readonly),
          location(loc) {}
};

struct TypeSymbol {
    std::string name;
    std::shared_ptr<Type> type;
    Location location;

    TypeSymbol(std::string name, std::shared_ptr<Type> type, Location loc = {})
        : name(std::move(name)), type(std::move(type)), location(loc) {}
};

struct RoutineSymbol {
    std::string name;
    std::vector<VariableSymbol> parameters;
    std::shared_ptr<Type> return_type;
    bool is_forward = false;
    bool is_defined = false;
    Location location;

    RoutineSymbol(std::string name,
                  std::vector<VariableSymbol> params,
                  std::shared_ptr<Type> ret_type,
                  bool is_forward = false,
                  bool is_defined = false,
                  Location loc = {})
        : name(std::move(name)),
          parameters(std::move(params)),
          return_type(std::move(ret_type)),
          is_forward(is_forward),
          is_defined(is_defined),
          location(loc) {}

    bool is_procedure() const {
        return return_type && return_type->is_void();
    }
    bool is_function() const {
        return !is_procedure();
    }
};