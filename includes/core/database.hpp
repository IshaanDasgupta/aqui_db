#pragma once

#include "core/table.hpp"
#include "core/types.hpp"
#include "core/exception.hpp"
#include <vector>
#include <string>
#include <optional>
#include <tl/expected.hpp>
#include <map>

namespace core {
class Table;

class Database {
public:
    Database();

    tl::expected<void, DatabaseException> createTable(
        const std::string& table_name,
        const std::vector<ColumnSchema>& columns,
        int pk_idx
    );

    tl::expected<void, DatabaseException> insert(
        const std::string& table_name,
        const Record& vals
    );

    tl::expected<Record, DatabaseException> get(
        const std::string& table_name,
        const Field& key
    ) const;

    tl::expected<std::vector<Record>, DatabaseException> getAll(
        const std::string& table_name
    ) const;


private:
    std::map<std::string, core::Table> tables;
};

}
