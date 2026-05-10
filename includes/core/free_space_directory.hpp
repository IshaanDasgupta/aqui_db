#pragma once
#include "types/types.hpp"

namespace core{

class FreeSpaceDirectory{
public:
    bool checkFreeSpace(uint16_t target_size);
    bool consumeFreeSpace(uint16_t target_size);

    bool checkFreePage();
    uint32_t consumeFreePage();
private:
};

}