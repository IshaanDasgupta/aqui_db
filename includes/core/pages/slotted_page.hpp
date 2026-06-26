#pragma once

#include "types/types.hpp"
#include "core/buffer_pool_manager.hpp"
#include "core/free_space_directory.hpp"

namespace core{

//TODO: deletion and update(deletion+insert)
class SlottedPageManager{
public:
    // returns the deserialized tuple at the given offset in the page with given page_id
    static types::Tuple readTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, const uint16_t offset);
    static std::optional<types::Tuple> findTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, std::string& key);
    // writes the serialized tuple in the given page_id and returns it's offset
    static uint16_t writeTuple(
        types::Frame* frame,
        const types::Tuple& tuple,
        const uint32_t key_overfow_page_id,
        const uint32_t value_overflow_page_id
    );
    // soft delets the kv pair at the given offset in the given page_id
    static void deleteTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, const uint32_t offset);
    
    static void serializeSlottedPageHeader(char* buffer, const types::SlottedPageHeader& header);
    static types::SlottedPageHeader deserializeSlottedPageHeader(const char* buffer);
private:
    SlottedPageManager();
    static void compaction(const char (&buffer)[config::PAGE_SIZE]);
};

}