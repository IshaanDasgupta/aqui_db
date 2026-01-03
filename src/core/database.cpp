#include "core/database.hpp"
#include "core/table.hpp"
#include "core/types.hpp"
#include "core/exception.hpp"
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <tl/expected.hpp>

namespace core {
Database::Database() = default;

tl::expected<void, DatabaseException> Database::createTable(const std::string& table_name, const std::vector<core::ColumnSchema>& columns, int pk_idx){
    std::map<std::string, core::Table>::iterator it = tables.find(table_name);
    if (it != tables.end()) {
        return tl::unexpected(DuplicateTable(table_name));
    }

    tl::expected<core::Table, core::DatabaseException> table_creation_result = core::Table::create(table_name, columns, pk_idx);

    if (!table_creation_result){
        return tl::unexpected(table_creation_result.error());
    }

    tables.emplace(table_name, std::move(*table_creation_result));
    return {};
};

tl::expected<void, DatabaseException> Database::insert(const std::string& table_name, const core::Record& vals){
    std::map<std::string, core::Table>::iterator it = tables.find(table_name);
    if (it == tables.end()) {
        return tl::unexpected(TableNotFoundException(table_name));
    }

    core::Table& table = it->second;
    return table.insert(vals);
};

tl::expected<Record, DatabaseException> Database::get(const std::string& table_name, const Field& key) const {
    std::map<std::string, core::Table>::const_iterator it = tables.find(table_name);
    if (it == tables.end()) {
        return tl::unexpected(TableNotFoundException(table_name));
    }
    
    const core::Table& table = it->second;
    return table.get(key);
};

tl::expected<std::vector<Record>, DatabaseException> Database::getAll(const std::string& table_name) const {
    std::map<std::string, core::Table>::const_iterator it = tables.find(table_name);
    if (it == tables.end()) {
        return tl::unexpected(TableNotFoundException(table_name));
    }
    
    const core::Table& table = it->second;
    return table.getAll();
};

};
