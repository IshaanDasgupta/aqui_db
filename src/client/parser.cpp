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

void Parser::paserAndExecute(const std::string& command, core::Database& db){
    tl::expected<std::vector<client::Token>, client::ClientException> out = client::Lexer::tokenize(command);
    if (!out){
        std::cout << out.error().what() << "\n";
        return;
    };

    std::vector<client::Token> tokens = *out;
    for (auto i:tokens){
        std::cout << utils::to_string(i) << "\n";
    }
    return;
};

}