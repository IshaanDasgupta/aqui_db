#pragma once

#include <exception>
#include <string>
#include <core/types.hpp>

namespace utils{
    std::string to_string(const core::Field& f);
    std::string to_string(const core::FieldSchema t);
}

namespace core {

class DatabaseException : public std::exception {
public:
    explicit DatabaseException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
private:
    std::string message;
};

class TableNotFoundException : public DatabaseException {
public:
    TableNotFoundException(const std::string& table_name)
        : DatabaseException("Table not found " + table_name) {}
};

class RecordNotFoundException : public DatabaseException {
public:
    RecordNotFoundException(const std::string& table_name, const Field& record_key)
        : DatabaseException(
            "Record not found in " + table_name + " with key " + utils::to_string(record_key)
        ) {}
};

class DuplicateTable : public DatabaseException {
public:
    DuplicateTable(const std::string& table_name)
        : DatabaseException(
            "Table with name '" + table_name + "' already exists in the database"
        ) {}
};

class DuplicateKey : public DatabaseException {
public:
    using DatabaseException::DatabaseException;
};

class InvalidTableSchema : public DatabaseException {
public:
    using DatabaseException::DatabaseException;
};

class InvalidRecord : public DatabaseException {
public:
    using DatabaseException::DatabaseException;
};

class InvalidFieldType : public DatabaseException {
public:
    using DatabaseException::DatabaseException;
};

}