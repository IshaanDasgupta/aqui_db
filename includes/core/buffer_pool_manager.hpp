#pragma once

#include "core/page_manager.hpp"
#include "types/types.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace core{

// add eviction, dirty page and pinning logic
// page compaction loggic where ?
class Buffer_Pool_Manager{
public:
    Buffer_Pool_Manager();
    // returns a read-only frame pointer with the given page_id from buffer_pool if not present, fetches it from the disk
    const types::Frame* fetch_page(const uint32_t page_id);
    // returns a read-write frame pointer with the given page_id from buffer_pool if not present, fetches it from the disk
    types::Frame* fetch_page_mut(uint32_t page_id);
    // flushes the frame with given page_id to disk
    void flush_page(const uint32_t page_id);

private:
    // frees a single frame from buffer pool (flushes it if it is dirty) and returns the index of buffer pool which is now free.
    uint32_t free_buffer_pool();
    // keeps a cache of pages in raw binary format
    std::vector<types::Frame> page_buffer_;
    // used to get the index of the page_buffer where the page with page_id is present
    std::unordered_map<uint32_t, uint32_t> page_id_to_idx_;
    // used to get the offset for any page_id in the disk file
    // Page_Directory page_directory_;
    // to read/write data to and from disk
    Page_Manager page_manger_;
};

}

