#pragma once

#include "types/types.hpp"

namespace core
{

class PageDirectory{
public:
    uint32_t getNextPageId();
    void incrementTotalPages();
private:
};

}
