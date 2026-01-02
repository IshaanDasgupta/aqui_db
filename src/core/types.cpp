#include "core/types.hpp"
#include "core/exception.hpp"

#include <tl/expected.hpp>
#include <stdexcept>

namespace core {

ColumnSchema::ColumnSchema(const std::string& name, FieldSchema type)
    : name(name), type(type) {}

Field to_field(const std::string& s) {
    if (s == "true")  return true;
    if (s == "false") return false;

    try {
        size_t idx;
        int v = std::stoi(s, &idx);
        if (idx == s.size())
            return v;
    } catch (...) {}

    return s;
}

tl::expected<FieldSchema, DatabaseException> to_field_schema(const std::string& s) {
    if (s == "int") return FieldSchema::Int;
    else if (s == "bool") return FieldSchema::Bool;
    else if (s == "string") return FieldSchema::String;
    else return tl::unexpected(InvalidFieldType("Invalid field type '" + s + "'"));
}

FieldSchema getFieldSchemaType(const Field& f) {
    switch (f.index()) {
        case 0: return FieldSchema::Int;
        case 1: return FieldSchema::Bool;
        case 2: return FieldSchema::String;
        default:
            throw std::logic_error("Invalid Field variant index");
    }
}

bool compare(const Field& a, const Field& b) {
    return a.index() == b.index();
}

bool matchesSchemaType(const Field& f, FieldSchema t) {
    return getFieldSchemaType(f) == t;
}

}
