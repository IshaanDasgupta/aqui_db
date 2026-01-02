#pragma once

#include "core/database.hpp"
#include "client/exception.hpp"

#include <string>
#include <vector>

namespace client{

class Parser{
public:
    static void paserAndExecute(const std::string& commnd, core::Database& db);
};

}