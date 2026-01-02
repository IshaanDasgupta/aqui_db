#include "utils/to_string.hpp"
#include "core/database.hpp"   
#include <variant>

namespace utils {

std::string to_string(const core::Field& f) {
    return std::visit([](const auto& value) -> std::string {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) return std::to_string(value);
        else if constexpr (std::is_same_v<T, bool>) return value ? "true" : "false";
        else return value;
    }, f);
}

std::string to_string(const core::FieldSchema t) {
    switch (t) {
        case core::FieldSchema::Int:    return "int";
        case core::FieldSchema::Bool:   return "bool";
        case core::FieldSchema::String: return "string";
        default:                        return "unknown";
    }
}

}