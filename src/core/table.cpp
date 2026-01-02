#include "core/table.hpp"
#include "core/types.hpp"
#include "core/exception.hpp"
#include <stdexcept>
#include <variant>
#include <tl/expected.hpp>
#include "utils/to_string.hpp"

namespace core {

Table::Table(const std::string& table_name, const std::vector<core::ColumnSchema>& columns, int pk_idx){
    this->table_name = table_name;
    this->columns= columns;
    this->pk_idx = pk_idx;
};

tl::expected<Table, DatabaseException> Table::create(const std::string& table_name, const std::vector<core::ColumnSchema>& columns, int pk_idx){
    if (pk_idx < 0 || pk_idx >= columns.size())
        return tl::unexpected(InvalidTableSchema("Invalid primary key index"));

    return Table(table_name, columns, pk_idx);
}

tl::expected<void, DatabaseException> Table::insert(const core::Record& vals){
    if (vals.size() != this->columns.size()) {
        return tl::unexpected(InvalidRecord("Value count does not match column count"));
    };

    Record record;
    for (size_t i = 0; i < columns.size(); i++) {
        const core::ColumnSchema& columnSchema = columns[i];
        const core::Field& val = vals[i];

        if (!matchesSchemaType(val, columnSchema.type)){
            return tl::unexpected(
                InvalidRecord(
                    "Table '" + this->table_name + "', column '" + columnSchema.name +
                    "': value '" + utils::to_string(val) +
                    "' does not match expected type '" + utils::to_string(columnSchema.type) + "'"
                )
            );
        };

        record.push_back(val);
    }

    data.push_back(record);
    return {};
}

//TODO: check for type match
tl::expected<Record, DatabaseException> Table::get(const Field& key) const {
    for (Record record:data){
        if (record[pk_idx] == key){
            return record;
        }
    }    
    return tl::unexpected(RecordNotFoundException(this->table_name, key));
}

std::vector<Record> Table::getAll() const {
    return data;
}

};
