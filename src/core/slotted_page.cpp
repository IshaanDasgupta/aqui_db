#include "core/slotted_page.hpp"
#include "core/buffer_pool_manager.hpp"
#include "core/overflow_page.hpp"
#include "types/types.hpp"
#include <stdexcept>

types::Tuple core::SlottedPageManager::readTuple(core::Buffer_Pool_Manager& buffer_pool_manager,const uint32_t page_id, const uint32_t offset){
    const types::Frame* frame = buffer_pool_manager.fetchPage(page_id);
    types::SlottedPage page{frame->buffer};
    const types::SlottedPageHeader& header = *page.header();    

    if(offset < sizeof(types::SlottedPageHeader) || offset >= header.record_start_offset){
        throw std::runtime_error("Illegal offset");
    }

    size_t pos = offset;
    uint32_t key_size;
    std::memcpy(&key_size, page.buffer + pos, sizeof(key_size));
    pos += sizeof(key_size);

    uint8_t key_overflow;
    std::memcpy(&key_overflow, page.buffer + pos, sizeof(key_overflow));
    pos += sizeof(key_overflow);

    std::string key;
    if (key_overflow == true){
        uint32_t curr_overflow_page_id;
        std::memcpy(&curr_overflow_page_id, page.buffer + pos, sizeof(curr_overflow_page_id));
        pos += sizeof(curr_overflow_page_id);

        key = core::OverflowPageManager::getOverflowValue(buffer_pool_manager, curr_overflow_page_id);
    }else{
        key.assign(page.buffer + pos, key_size);
        pos += key_size;
    }

    uint32_t value_size;
    std::memcpy(&value_size, page.buffer + pos, sizeof(value_size));
    pos += sizeof(value_size);

    uint8_t value_overflow;
    std::memcpy(&value_overflow, page.buffer + pos, sizeof(value_overflow));
    pos += sizeof(value_overflow);

    std::string value;
    if (value_overflow == true){
        uint32_t curr_overflow_page_id;
        std::memcpy(&curr_overflow_page_id, page.buffer + pos, sizeof(curr_overflow_page_id));
        pos += sizeof(curr_overflow_page_id);

        value = core::OverflowPageManager::getOverflowValue(buffer_pool_manager, curr_overflow_page_id);
    }else{
        value.assign(page.buffer + pos, value_size);
        pos += value_size;
    }

    return types::Tuple{key, value};
};


uint32_t core::SlottedPageManager::writeTuple(
        core::FreeSpaceDirectory free_space_directory,
        core::PageDirectory page_directory,
        core::Buffer_Pool_Manager& buffer_pool_manager,
        const uint32_t page_id,
        const types::Tuple& tuple
    ){
    // types::Frame* frame = buffer_pool_manager.fetchPageMut(page_id);
    // types::SlottedPage page{frame->buffer};
    // types::SlottedPageHeader& header = *page.header();

    // // compute key, value and meta-data sizes
    // uint32_t key_size;
    // uint8_t key_overfow;
    // std::string key;
    // uint32_t key_overfow_page_id;
    // if (tuple.key.size() * sizeof(char) > config::OVERFLOW_LIMIT){
    //     key_size = sizeof(key_overfow_page_id);
    //     key_overfow = 1;
    // }else{
    //     key_size = tuple.key.size() * sizeof(char);
    //     key_overfow = 0;
    //     key = tuple.key;
    // }

    // uint32_t value_size;
    // uint8_t value_overfow;
    // std::string value;    
    // uint32_t value_overfow_page_id;
    // if (tuple.val.size() * sizeof(char) > config::OVERFLOW_LIMIT){
    //     value_size = sizeof(value_overfow_page_id);
    //     value_overfow = 1;
    // }else{
    //     value_size = tuple.val.size() * sizeof(char);
    //     value_overfow = 0;
    //     value = tuple.val;
    // }

    // // check whether the tuple can fit into the current page
    // uint32_t continuous_free_space = header.record_start_offset - header.slot_end_offset;
    // uint32_t total_free_space = continuous_free_space + header.fragmented_bytes;

    // uint32_t record_size = sizeof(uint32_t) // key_size field
    //                     + sizeof(uint8_t)   // key_overflow flag
    //                     + key_size          // key data or overflow page id
    //                     + sizeof(uint32_t)  // value_size field
    //                     + sizeof(uint8_t)   // value_overflow flag
    //                     + value_size;       // value data or overflow page id

    // uint32_t tuple_size = record_size + sizeof(uint32_t);

    // if (tuple_size > total_free_space){
    //     throw std::runtime_error("Could not fit tuple into given page");
    //     return;
    // }

    // if (tuple_size > continuous_free_space){
    //     // compaction();
    // }

    // // create overflow pages for key and value if required
    // if (key_overfow){
    //     // make overflow pages

    //     key_overfow_page_id = createOverflowPage();
    // }

    // if (value_overfow){
    //     // make overflow pages
    //     value_overfow_page_id = 1;
    // }
    
    // // start writing record backwards
    // size_t pos = header.record_start_offset;
    // pos -= value_size;
    // memcpy(
    //     page.buffer + pos,
    //     value_overfow
    //         ? static_cast<const void*>(&value_overfow_page_id)
    //         : static_cast<const void*>(value.data()),
    //     value_size
    // );

    // pos -= sizeof(value_overfow);
    // memcpy(page.buffer + pos, &value_overfow, sizeof(value_overfow));

    // pos -= sizeof(value_size);
    // memcpy(page.buffer + pos, &value_size, sizeof(value_size));

    // pos -= key_size;
    // memcpy(
    //     page.buffer + pos,
    //     key_overfow
    //         ? static_cast<const void*>(&key_overfow_page_id)
    //         : static_cast<const void*>(key.data()),
    //     key_size
    // );

    // pos -= sizeof(key_overfow);
    // memcpy(page.buffer + pos, &key_overfow, sizeof(key_overfow));

    // pos -= sizeof(key_size);
    // memcpy(page.buffer + pos, &key_size, sizeof(key_size));

    // // write slot offset
    // memcpy(page.buffer + header.slot_end_offset, &pos, sizeof(pos));

    // // update header values
    // header.record_start_offset = pos;
    // header.slot_end_offset += sizeof(pos);
    // header.tuple_count++;

    // // mark the page dirty so that it gets flused to disk when it is evicted from the buffer pool
    // frame->is_dirty = 1;

    // // return the start of the currently written slot
    // return header.slot_end_offset - sizeof(pos);    
}

uint32_t core::SlottedPageManager::deleteTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, const uint32_t offset){

};