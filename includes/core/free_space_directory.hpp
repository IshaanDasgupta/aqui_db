#pragma once
#include "types/types.hpp"

namespace core{

class FreeSpaceDirectory{
public:
    uint32_t getFreePage();
    types::FreePage consumeFreePage();
private:
};

}