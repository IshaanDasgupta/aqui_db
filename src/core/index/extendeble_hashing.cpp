#include "core/index/extendeble_hashing.hpp"
#include "types/types.hpp"
#include "core/pages/bucket_directory_page.hpp"
#include <xxhash.h>

core::HashTable::HashTable(core::Buffer_Pool_Manager& buffer_pool_manager, uint32_t init_page_id): buffer_pool_manager_(buffer_pool_manager)
{
    init_page_id = init_page_id;
    const types::Frame* frame = buffer_pool_manager_.fetchPage(init_page_id);
};

  
void core::HashTable::insert(std::string &key, std::pair<uint32_t, uint16_t> value)
{
    uint64_t hash = XXH64(key.data(), key.size(), 0);
    uint32_t hash_mask = (hash & ((uint32_t)1 << global_depth) - 1);
}