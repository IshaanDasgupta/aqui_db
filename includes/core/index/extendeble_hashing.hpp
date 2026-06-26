#pragma once

#include "core/buffer_pool_manager.hpp"

namespace core{

//TODO create the init page in constructor and return the page_id
class HashTable{
public:
    HashTable(core::Buffer_Pool_Manager& buffer_pool_manager, uint32_t init_page_id);
    std::pair<uint32_t, uint16_t> get(std::string& key);
    void insert(std::string& key, std::pair<uint32_t, uint16_t> value);
private:
    //page_id of the first page in the bucket directory 
    uint32_t init_page_id;
    // global_depth of bits taken from the hash
    uint16_t global_depth;

    core::Buffer_Pool_Manager& buffer_pool_manager_;
};


}