#pragma once

#include "core/buffer_pool_manager.hpp"

namespace core{

class HashTable{
public:
    HashTable(core::Buffer_Pool_Manager& buffer_pool_manager, uint32_t init_page_id);
private:
    //page_id of the first page in the bucket directory 
    uint32_t init_page_id;
    // global_depth of bits taken from the hash
    uint16_t global_depth;

    core::Buffer_Pool_Manager& buffer_pool_manager_;
};


}