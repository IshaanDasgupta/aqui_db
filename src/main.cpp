#include <iostream>
#include "core/database.hpp"
#include "client/parser.hpp"
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

        tl::expected<std::vector<client::Token>, client::ClientException> out = client::Lexer::tokenize(command);
        if (!out){
            std::cout << out.error().what() << "\n";
            continue;
        };

        std::vector<client::Token> tokens = *out;
        // for (client::Token i:tokens){
        //     std::cout << utils::to_string(i) << "\n";
        // }

        client::Parser* parser = new client::Parser(tokens);
        auto ast = parser->parse();

        if (!ast) {
            std::cerr << ast.error().what() << "\n";
            continue;
        }

        parser->printAST(*ast);

    }
    
    return 0;
}
