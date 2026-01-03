#include "core/database.hpp"
#include "client/parser.hpp"
#include "utils/helper.hpp"
#include "client/exception.hpp"
#include "client/command.hpp"
#include "utils/to_string.hpp"
#include <tl/expected.hpp>
#include <client/lexer.hpp>
#include <iostream>

namespace client{

Parser::Parser(std::vector<client::Token>& tokens)
    : cursor(0), tokens(tokens) {}


tl::expected<client::ASTNode*, client::ClientException> Parser::parse(){
    ASTNode* queries = new ASTNode();
    queries->type = ASTNodeType::QUERIES;

    while(cursor < tokens.size() && tokens[cursor].type != TokenType::TK_EOF){
        switch (tokens[cursor].type)
        {
            case TokenType::TK_CREATE: {
                ASTNode* createNode = new ASTNode();
                createNode->type = ASTNodeType::CREATE_STMT;
                advance();

                tl::expected<void, client::ClientException> out1 = expect({TokenType::TK_DATABASE, TokenType::TK_TABLE});
                if (!out1){
                    return tl::unexpected(out1.error());
                }

                if (tokens[cursor].type == TokenType::TK_DATABASE){
                    tl::expected<client::ASTNode*, client::ClientException> out = parseDatabaseDef();
                    if (!out){
                        return tl::unexpected(out.error());
                    }
                    createNode->children.push_back(*out);
                }
                else if (tokens[cursor].type == TokenType::TK_TABLE){
                    tl::expected<client::ASTNode*, client::ClientException> out = parserTableDef();
                    if (!out){
                        return tl::unexpected(out.error());
                    }
                    createNode->children.push_back(*out);         
                }
                else{
                    return tl::unexpected(client::ClientException(
                        "Expected DATABASE or TABLE after CREATE, got " + utils::to_string(tokens[cursor].type)
                    ));
                };

                tl::expected<void, client::ClientException> out2 = expectAndAdvance(TokenType::TK_SEMICOL);
                if (!out2){
                    return tl::unexpected(out2.error());
                }

                queries->children.push_back(createNode);
                break;
            }

            default: {
                return tl::unexpected(client::ClientException(
                    "Unexpected token at top level: " + utils::to_string(tokens[cursor].type) + ", expected CREATE statement"
                ));
            }
        }
    }

    return queries;
}

void Parser::advance(){
    this->cursor++;
}

tl::expected<void, client::ClientException> Parser::expect(client::TokenType type) const {
    if (this->cursor >= tokens.size() || this->tokens[cursor].type != type){
        return tl::unexpected(ClientException(
            "Unexpected token: expected " + utils::to_string(type) +
            ", got " + (this->cursor < tokens.size() ? utils::to_string(tokens[cursor].type) : "EOF")
        ));
    }

    return {};
}

tl::expected<void, client::ClientException> Parser::expect(const std::vector<client::TokenType> types) const {
    if (this->cursor >= tokens.size()){
        return tl::unexpected(ClientException("Unexpected end of input, expected one of multiple token types"));
    }

    bool good = false;
    for (client::TokenType candidate: types){
        if (this->tokens[cursor].type == candidate){
            return {};
        }
    }

    std::string expected_tokens = "{ ";
    for (auto t : types) expected_tokens += utils::to_string(t) + " ";
    expected_tokens += "}";

    return tl::unexpected(ClientException(
        "Unexpected token: expected one of " + expected_tokens +
        ", got " + utils::to_string(tokens[cursor].type)
    ));
}

tl::expected<void, client::ClientException> Parser::expectAndAdvance(client::TokenType type){
    if (this->cursor >= tokens.size() || this->tokens[cursor].type != type){
        return tl::unexpected(ClientException(
            "Unexpected token: expected " + utils::to_string(type) +
            ", got " + (this->cursor < tokens.size() ? utils::to_string(tokens[cursor].type) : "EOF")
        ));
    }

    advance();
    return {};
};

tl::expected<void, client::ClientException> Parser::expectAndAdvance(const std::vector<client::TokenType> types){
    if (this->cursor >= tokens.size()){
        return tl::unexpected(ClientException("Unexpected end of input, expected one of multiple token types"));
    }

    bool good = false;
    for (client::TokenType candidate: types){
        if (this->tokens[cursor].type == candidate){
            advance();
            return {};
        }
    }

    std::string expected_tokens = "{ ";
    for (auto t : types) expected_tokens += utils::to_string(t) + " ";
    expected_tokens += "}";

    return tl::unexpected(ClientException(
        "Unexpected token: expected one of " + expected_tokens +
        ", got " + utils::to_string(tokens[cursor].type)
    ));
};


tl::expected<client::ASTNode*, client::ClientException> Parser::parseDatabaseDef(){
    tl::expected<void, client::ClientException> out = expectAndAdvance(TokenType::TK_DATABASE);
    if (!out){
        return tl::unexpected(out.error());
    }

    ASTNode* databaseDefNode = new ASTNode();
    databaseDefNode->type = ASTNodeType::DATABASE_DEF;

    tl::expected<void, client::ClientException> out2 = expect(TokenType::TK_IDENT);
    if (!out2){
        return tl::unexpected(out2.error());
    }    

    databaseDefNode->payload = tokens[cursor].value;
    advance();

    return databaseDefNode;
};

tl::expected<client::ASTNode*, client::ClientException> Parser::parserTableDef(){
    tl::expected<void, client::ClientException> out1 = expectAndAdvance(TokenType::TK_TABLE);
    if (!out1){
        return tl::unexpected(out1.error());
    }

    ASTNode* tableDefNode = new ASTNode();
    tableDefNode->type = ASTNodeType::TABLE_DEF;

    tl::expected<void, client::ClientException> out2 = expect(TokenType::TK_IDENT);
    if (!out2){
        return tl::unexpected(out2.error());
    }    

    tableDefNode->payload = tokens[cursor].value;
    advance();

    tl::expected<void, client::ClientException> out3 = expectAndAdvance(TokenType::TK_LPAREN);
    if (!out3){
        return tl::unexpected(out3.error());
    }

    while(true){
        tl::expected<client::ASTNode*, client::ClientException> out = parserColDef();
        if (!out){
            return tl::unexpected(out.error());
        }
        tableDefNode->children.push_back(*out);

        tl::expected<void, client::ClientException> out2 = expect(TokenType::TK_COMMA);
        if (!out2){
            break;
        }

        advance();
    }

    tl::expected<void, client::ClientException> out = expectAndAdvance(TokenType::TK_RPAREN);
    if (!out){
        return tl::unexpected(out.error());
    }

    return tableDefNode;
};

tl::expected<client::ASTNode*, client::ClientException> Parser::parserColDef(){
    ASTNode* colDefNode = new ASTNode();
    colDefNode->type = ASTNodeType::COL_DEF;

    tl::expected<client::ASTNode*, client::ClientException> out1 = parseDatatype();
    if (!out1){
        return tl::unexpected(out1.error());
    }
    colDefNode->children.push_back(*out1);

    tl::expected<void, client::ClientException> out2 = expect(TokenType::TK_IDENT);
    if (!out2){
        return tl::unexpected(out2.error());
    }    

    colDefNode->payload = tokens[cursor].value;
    advance();

    return colDefNode;
};

tl::expected<client::ASTNode*, client::ClientException> Parser::parseDatatype() {
    ASTNode* datatypeNode = new ASTNode();
    datatypeNode->type = ASTNodeType::DATATYPE;

    tl::expected<void, client::ClientException> out = expect({TokenType::TK_NUMBER_TYPE, TokenType::TK_BOOL_TYPE, TokenType::TK_CHAR_TYPE});
    if (!out){
        return tl::unexpected(out.error());
    }    

    datatypeNode->payload = tokens[cursor].value;
    TokenType datatype = tokens[cursor].type;
    advance();

    if (datatype == TokenType::TK_CHAR_TYPE){
        tl::expected<void, client::ClientException> out1 = expectAndAdvance(TokenType::TK_LPAREN);
        if (!out1){
            return tl::unexpected(out1.error());
        }

        tl::expected<client::ASTNode*, client::ClientException> out2 = parseNumber();
        if (!out2){
            return tl::unexpected(out2.error());
        }
        datatypeNode->children.push_back(*out2);

        tl::expected<void, client::ClientException> out3 = expectAndAdvance(TokenType::TK_RPAREN);
        if (!out3){
            return tl::unexpected(out3.error());
        }
    }

    return datatypeNode;
}

tl::expected<client::ASTNode*, client::ClientException> Parser::parseNumber(){
    ASTNode* numberNode = new ASTNode();
    numberNode->type = ASTNodeType::NUMBER;

    tl::expected<void, client::ClientException> out = expect(TokenType::TK_NUMBER);
    if (!out){
        return tl::unexpected(out.error());
    }

    numberNode->payload = tokens[cursor].value;
    advance();

    return numberNode;
}


void Parser::printAST(const client::ASTNode* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; ++i)
        std::cout << "  ";

    std::cout << utils::to_string(node->type);

    if (!node->payload.empty()) {
        std::cout << " [" << node->payload << "]";
    }

    std::cout << "\n";

    for (const ASTNode* child : node->children) {
        printAST(child, indent + 1);
    }
}

}