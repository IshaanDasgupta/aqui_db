#pragma once

#include "core/database.hpp"
#include "client/exception.hpp"
#include <client/token.hpp>

#include <string>
#include <vector>

namespace client{

enum class ASTNodeType{
    QUERIES,
    CREATE_STMT,
    DATABASE_DEF,
    TABLE_DEF,
    COL_DEF,
    DATATYPE,
    NUMBER
};

class ASTNode{
public:
    client::ASTNodeType type;
    std::string payload;
    std::vector<client::ASTNode*> children;
};

class Parser{
public:
    explicit Parser(std::vector<client::Token>& tokens);
    tl::expected<client::ASTNode*, client::ClientException> parse();
    void printAST(const client::ASTNode* node, int indent = 0);
private:
    std::size_t cursor;
    std::vector<client::Token> &tokens;
    void advance();
    tl::expected<void, client::ClientException> expect(client::TokenType type) const;
    tl::expected<void, client::ClientException> expect(const std::vector<client::TokenType> type) const;
    tl::expected<void, client::ClientException> expectAndAdvance(client::TokenType type);
    tl::expected<void, client::ClientException> expectAndAdvance(const std::vector<client::TokenType> type);
    tl::expected<client::ASTNode*, client::ClientException> parseDatabaseDef();
    tl::expected<client::ASTNode*, client::ClientException> parserTableDef();
    tl::expected<client::ASTNode*, client::ClientException> parserColDef();
    tl::expected<client::ASTNode*, client::ClientException> parseDatatype();
    tl::expected<client::ASTNode*, client::ClientException> parseNumber();

};

}