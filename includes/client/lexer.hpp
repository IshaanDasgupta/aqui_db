#pragma once

#include <vector>
#include <string>
#include <tl/expected.hpp>
#include <client/token.hpp>

namespace client {

class ClientException;

class Lexer {
public:
    static tl::expected<std::vector<client::Token>, client::ClientException> tokenize(const std::string &cargo);
private:
};

}