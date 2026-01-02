#pragma once

#include <variant>
#include <string>
#include <vector>
#include <tl/expected.hpp>

namespace core {

class DatabaseException;

using Field  = std::variant<int, bool, std::string>;
using Record = std::vector<Field>;

enum class FieldSchema { Int, Bool, String };

struct ColumnSchema {
    std::string name;
    FieldSchema type;

    ColumnSchema(const std::string& name, FieldSchema type);
};

Field to_field(const std::string& s);
tl::expected<FieldSchema, core::DatabaseException> to_field_schema(const std::string& s);

FieldSchema getFieldSchemaType(const Field& f);
bool matchesSchemaType(const Field& f, FieldSchema t);
bool compare(const Field& a, const Field& b);


}
