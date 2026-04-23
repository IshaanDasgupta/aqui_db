#pragma once

#include "core/buffer_pool_manager.hpp"

namespace core{
class StorageManger{
public:
    // gets the raw data of the record with given page_id and slot_offset and deserializes it
    types::Tuple get_record(const uint32_t page_id, const uint32_t slot_offset);
    // gets the raw data of all the records in the given page_id and deserializes it
    std::vector<types::Tuple> get_all_records(const uint32_t page_id);
    // serializes the record and writes it to a free slot present otherwise creates a new page and write it there and return the page_id in which the tuple was written
    uint32_t insert_record(types::Tuple record);
    // soft deletes the record with given page_id and slot_offset 
    void delete_record(const uint32_t page_id, const uint32_t slot_offset);


private:
    // used to get the page_ids of the pages associated with this query
    // Page_Index page_index;
    // used to get the page_id which has the next greater free space (than some input) among all pages in sorted order of free space
    // Free_Space_Directory free_space_directory_;
    // used to access the data actually stored in the KV store
    core::Buffer_Pool_Manager buffer_pool_manger_; 

};

}