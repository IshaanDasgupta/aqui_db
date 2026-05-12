#include "core/index/extendeble_hashing.hpp"
#include "types/types.hpp"

core::HashTable::HashTable(core::Buffer_Pool_Manager& buffer_pool_manager, uint32_t init_page_id): buffer_pool_manager_(buffer_pool_manager)
{
    this->init_page_id = init_page_id;
    const types::Frame* frame = buffer_pool_manager_.fetchPage(init_page_id);
    
};