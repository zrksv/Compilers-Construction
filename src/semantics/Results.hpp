#pragma once
#include <string>
#include "types/Type.hpp"

enum class Access {
    Value,
    Writable,
    ReadOnly
};

struct ExprResult {
    TypeRef type = nullptr;
    Access access = Access::Value;
    std::string readonly_reason;

    static ExprResult value(TypeRef t) {
        return ExprResult{std::move(t), Access::Value, ""};
    }

    static ExprResult writable(TypeRef t) {
        return ExprResult{std::move(t), Access::Writable, ""};
    }

    static ExprResult readonly(TypeRef t, std::string reason = "") {
        return ExprResult{std::move(t), Access::ReadOnly, std::move(reason)};
    }

    static ExprResult error() {
        return ExprResult{nullptr, Access::Value, ""};
    }

    bool is_error() const {
        return type == nullptr;
    }
};

struct StmtResult {
    bool returns = false;
};
