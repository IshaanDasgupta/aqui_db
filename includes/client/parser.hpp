#pragma once

#include <string>
#include "types/types.hpp"

namespace client{

class Parser{
public:
    // would return a query of type (PUT, GET, GET_ALL, DEL) with associated data from the text input
    static types::Query parse(const std::string& input);
private:

};    

}