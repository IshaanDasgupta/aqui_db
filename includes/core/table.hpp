#pragma once

#include "core/types.hpp"
#include "core/exception.hpp"
#include <vector>
#include <string>
#include <optional>
#include <tl/expected.hpp>

namespace core {

class Table {
public:
    static tl::expected<Table, DatabaseException> create(const std::string& table_name, const std::vector<core::ColumnSchema>& columns, int pk_idx);
    tl::expected<void, DatabaseException> insert(const core::Record& vals);
    tl::expected<Record, DatabaseException> get(const Field& key) const;
    std::vector<Record> getAll() const;

private:
    Table(const std::string& table_name, const std::vector<core::ColumnSchema>& columns, int pk_idx);
    std::string table_name;
    std::vector<core::ColumnSchema> columns;
    int pk_idx;
    std::vector<Record> data;
};

}
