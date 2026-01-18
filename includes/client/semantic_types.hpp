#pragma once
#include <string>
#include <vector>
#include <variant>
#include <optional>

namespace client {

struct Data {
    enum { NUMBER, STRING, BOOL } type;
    std::variant<int, std::string, bool> val;
};

struct Expr {
    std::string colName;
    Data data;
};

using WhereClause = std::optional<Expr>;

using ColVal = std::vector<Data>;

using ColValList = std::vector<ColVal>;

using IdentList = std::vector<std::string>;

using AssginList = std::vector<Expr>;

struct Datatype {
    enum { INT, BOOL, CHAR } kind;
    int size = 0;
};

struct ColDef {
    Datatype type;
    std::string name;
};

struct TableDef {
    std::string name;
    std::vector<ColDef> cols;
};

using DatabaseDef = std::string;

//statements
struct CreateStmt {
    enum { DATABASE, TABLE } type;
    std::variant<DatabaseDef, TableDef> def;
};

struct InsertStmt {
    std::string table;
    ColValList rows;
};

struct SelectStmt {
    std::string table;
    IdentList colNames;
    std::optional<Expr> whereClause;
};

struct UpdateStmt {
    std::string table;
    std::vector<Expr> assingOps;
    std::optional<Expr> whereClause;
};

struct DeleteStmt {
    std::string table;
    std::optional<Expr> whereClause;
};

using Query = std::variant<CreateStmt, InsertStmt, SelectStmt, UpdateStmt, DeleteStmt>;

using QueryList = std::vector<Query>;


}
