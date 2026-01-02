#include <iostream>
#include "core/database.hpp"
#include "client/parser.hpp"

int main() {
    core::Database db;
    
    while(true){
        std::string command;
        std::getline(std::cin, command);

        if (command == "exit"){
            break;
        }

        client::Parser::paserAndExecute(command, db);
    }
    
    return 0;
}
