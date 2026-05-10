#pragma once

#include "types/types.hpp"
#include "core/buffer_pool_manager.hpp"

namespace core{

//TODO: deletion and update(deletion+insert)
class SlottedPageManager{
public:
    // returns the deserialized tuple at the given offset in the page with given page_id
    static types::Tuple readTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, const uint16_t offset);
    // writes the serialized tuple in the given page_id and returns it's offset
    static uint32_t writeTuple(
        core::FreeSpaceDirectory free_space_directory,
        core::Buffer_Pool_Manager& buffer_pool_manager,
        const types::Tuple& tuple
    );
    // soft delets the kv pair at the given offset in the given page_id and returns to total number of bytes freed
    static uint32_t deleteTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, const uint32_t offset);
    
    static void serializeSlottedPageHeader(char (&buffer)[config::SLOTTED_HEADER_SIZE], const types::SlottedPageHeader& header);
    static types::SlottedPageHeader deserializeSlottedPageHeader(const char* buffer);
private:
    SlottedPageManager();
    static void compaction(const char (&buffer)[config::PAGE_SIZE]);
};

}