#pragma once

#include "core/database.hpp"
#include "client/exception.hpp"
#include <tl/expected.hpp>
#include <client/token.hpp>
#include <client/semantic_types.hpp>
#include <string>
#include <vector>

namespace client{

class TokenListParser{
public:
    static tl::expected<client::QueryList, client::ClientException> parse(const std::vector<client::Token>& toks);
private:
};

}

