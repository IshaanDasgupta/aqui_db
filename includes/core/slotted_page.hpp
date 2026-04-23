#pragma once

#include "types/types.hpp"

namespace core{

class SlottedPageManager{
public:
    // returns the deserialized tuple at the given offset in the page with given page_id
    static types::Tuple readTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, const uint32_t offset);
    // writes the serialized tuple in the given page_id and returns it's offset
    static uint32_t writeTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, const types::Tuple& tuple);
    static uint32_t deleteTuple(const char (&buffer)[config::PAGE_SIZE],  const uint32_t offset);
private:
    SlottedPageManager();
    static void compaction(const char (&buffer)[config::PAGE_SIZE]);
};

}