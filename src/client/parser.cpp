#include "core/database.hpp"
#include "client/parser.hpp"
#include "utils/helper.hpp"
#include "client/exception.hpp"
#include "client/command.hpp"
#include "utils/to_string.hpp"
#include <tl/expected.hpp>
#include <client/lexer.hpp>
#include <client/semantic_types.hpp>
#include <client/parser_combinators.hpp>
#include <iostream>

namespace client{

Parser<client::Token> token(client::TokenType expectedType) {
    return Parser<client::Token>{
        [expectedType](size_t& cursor, const std::vector<Token>& tokens)
            -> tl::expected<client::Token, client::ClientException>
        {
            if (cursor >= tokens.size())
                return tl::unexpected(client::ClientException(
                    "Expected token " + utils::to_string(expectedType) + " got EOF"));

            if (tokens[cursor].type != expectedType)
                return tl::unexpected(client::ClientException(
                    "Expected token " + utils::to_string(expectedType) +
                    " got " + utils::to_string(tokens[cursor].type)));

            return tokens[cursor++];
        },
        utils::to_string(expectedType)
    };
}


auto ident = Parser<std::string>{
    token(TokenType::TK_IDENT).map([](const Token& t){ return t.value; }),
    "Ident"
};

auto number = Parser<int>{
    token(TokenType::TK_NUMBER).map([](const Token& t){ return std::stoi(t.value); }),
    "Number"
};

auto string = Parser<std::string>{
    token(TokenType::TK_STRING).map([](const Token& t){ return t.value.substr(1, t.value.size() - 2); }),
    "String"
};

auto datatype = Parser<client::Datatype>{ 
    client::choice<client::Datatype>({
        client::token(TokenType::TK_NUMBER_TYPE).map([](const client::Token&) { return Datatype{Datatype::INT}; }),
        client::token(TokenType::TK_BOOL_TYPE).map([](const client::Token&) { return Datatype{Datatype::BOOL}; }),
        client::seq(seq(token(TokenType::TK_CHAR_TYPE), token(TokenType::TK_LPAREN)), seq(number, token(TokenType::TK_RPAREN))).map([](auto p) { return Datatype{Datatype::CHAR, p.second.first}; })
    }),
    "Datatype"
};

auto data = Parser<client::Data>{
    client::choice<client::Data>({
        number.map([](auto p){ return Data{Data::NUMBER, p};}),
        string.map([](auto p){ return Data{Data::STRING, p};})
    }),
    "Data"
};

auto expr = Parser<client::Expr>{
    seq(
        seq(
            ident,
            token(TokenType::TK_EQ)
        ),
        data
    ).map([](auto p){ return Expr{p.first.first, p.second}; }),
    "Expr"
};

auto colVal = Parser<client::ColVal>{
    seq(
        seq(
            token(TokenType::TK_LPAREN),
            sep_by(data, token(TokenType::TK_COMMA))
        ),
        token(TokenType::TK_RPAREN)
    ).map([](auto p){ return p.first.second; }),
    "ColVal"
};

auto colValList = Parser<client::ColValList>{
    sep_by(
        colVal,
        token(TokenType::TK_COMMA)
    ),
    "ColValList"
};

auto identList = Parser<client::IdentList>{
    sep_by(
        ident,
        token(TokenType::TK_COMMA)
    ),
    "IdentList"
};

auto colDef = Parser<client::ColDef>{
    client::seq(datatype,ident).map([](auto p) { return ColDef{p.first, p.second}; }),
    "Coldef"
};

auto whereClause = Parser<client::WhereClause>{
    optional(
        seq(
            token(TokenType::TK_WHERE),
            expr
        )
    ).map([](auto p) -> client::WhereClause {
        if (p) return p->second;
        return std::nullopt;
    }),
    "WhereClause"
};

auto assginList = Parser<client::AssginList>{
    sep_by(
        expr,
        token(TokenType::TK_COMMA)
    )
};

auto tableDef = Parser<client::TableDef>{
    client::seq(
        client::seq(
            client::seq(
                token(TokenType::TK_TABLE),
                ident
            ),
            client::seq(
                token(TokenType::TK_LPAREN),
                client::sep_by(colDef, token(TokenType::TK_COMMA))
            )
        ),
        token(TokenType::TK_RPAREN)
    ).map([](auto p) { return TableDef{ p.first.first.second, p.first.second.second}; } ),
    "TableDef"
};

auto databaseDef = Parser<client::DatabaseDef>{ 
    client::seq(
        token(TokenType::TK_DATABASE),
        ident
    ).map([](auto p) { return DatabaseDef{p.second}; }),
    "DatabaseDef"
};

auto createStmt = Parser<client::CreateStmt>{ 
    client::seq(
        client::seq(
                token(TokenType::TK_CREATE),
                choice<client::CreateStmt>({
                    databaseDef.map([](auto p){ return CreateStmt{CreateStmt::DATABASE, p}; } ),
                    tableDef.map([](auto p){ return CreateStmt{CreateStmt::TABLE, p}; } ),
                })
            ).map([](auto p) { return p.second; }),
        token(TokenType::TK_SEMICOL)
    ).map([](auto p) {return p.first;}),
    "CreateStmt"
};

auto insertStmt = Parser<client::InsertStmt>{ 
    client::seq(
        client::seq(
            token(TokenType::TK_INSERT),
            ident
        ),
        client::seq(
            colValList,
            token(TokenType::TK_SEMICOL)
        )
    ).map([](auto p){ return InsertStmt{p.first.second, p.second.first}; }),
    "InsertStmt"
};

auto selectStmt = Parser<client::SelectStmt>{ 
    client::seq(
        client::seq(
            client::seq(
                token(TokenType::TK_SELECT),
                identList
            ),
            client::seq(
                ident,
                whereClause
            )
        ),
        token(TokenType::TK_SEMICOL)
    ).map([](auto p){ return SelectStmt{p.first.second.first, p.first.first.second}; }),
    "SelectStmt"
};

auto updateStmt = Parser<client::UpdateStmt>{ 
    client::seq(
        client::seq(
            client::seq(
                token(TokenType::TK_UPDATE),
                ident
            ),
            client::seq(
                assginList,
                whereClause
            )
        ),
        token(TokenType::TK_SEMICOL)
    ).map([](auto p){ return UpdateStmt{p.first.first.second, p.first.second.first, p.first.second.second}; }),
    "UpdateStmt"
};

auto deleteStmt = Parser<client::DeleteStmt>{ 
    client::seq(
        client::seq(
            token(TokenType::TK_DELETE),
            ident
        ),
        client::seq(
            whereClause,
            token(TokenType::TK_SEMICOL)
        )
    ).map([](auto p){ return DeleteStmt{p.first.second, p.second.first}; }),
    "DeleteStmt"
};

auto query = Parser<client::Query>{ 
    choice<client::Query>({
        createStmt.map([](auto p){ return Query{p}; }),
        insertStmt.map([](auto p){ return Query{p}; }),
        selectStmt.map([](auto p){ return Query{p}; }),
        updateStmt.map([](auto p){ return Query{p}; }),
        deleteStmt.map([](auto p){ return Query{p}; })
    }),
    "Query"
};

auto queryList = Parser<client::QueryList>{
    many(query),
    "QueryList"
};

tl::expected<QueryList, client::ClientException> TokenListParser::parse(const std::vector<client::Token>& toks) {
    std::size_t pos=0;
    auto res = queryList(pos, toks);
    return res;
};



}