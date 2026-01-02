#pragma once

#include <string>
#include <map>

namespace client {

enum class TokenType {
    #define X(name, regex) name,
    #include "token.def"
    #undef X

    #define K(name, value) name,
    #include "keyword.def"
    #undef K

    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};



}