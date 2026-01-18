#include <iostream>
#include "core/database.hpp"
#include "client/parser.hpp"
#include "client/semantic_types.hpp"
#include "client/lexer.hpp"
#include "utils/to_string.hpp"

int main() {
    core::Database db;
    
    while(true){
        std::string command;
        std::getline(std::cin, command);

        if (command == "exit"){
            break;
        }

        tl::expected<std::vector<client::Token>, client::ClientException> lexerOut = client::Lexer::tokenize(command);
        if (!lexerOut){
            std::cout << lexerOut.error().what() << "\n";
            continue;
        };

        std::cout << "Lexing done\n";

        std::vector<client::Token> tokens = *lexerOut;
        tokens.push_back(client::Token{client::TokenType::TK_EOF, ""});

        tl::expected<client::QueryList, client::ClientException> parserOut = client::TokenListParser::parse(tokens);
        if (!parserOut){
            std::cout << parserOut.error().what() << "\n";
            continue;
        };

        std::cout << "Parsing done\n";

    }
    
    return 0;
}
