#include "core/executor.hpp"
#include "core/index/extendeble_hashing.hpp"
#include "core/pages/overflow_page.hpp"
#include "core/pages/slotted_page.hpp"

void core::Executor::put_record(types::Tuple record){
    try{
        std::pair<uint32_t, uint16_t> present_page_data = page_index_.get(record.key);
        core::Executor::delete_record(present_page_data.first, present_page_data.second);
    }
    catch(...){

    }

    // compute key, value and meta-data sizes
    bool key_overflow_flag = (record.key.size() * sizeof(char) > config::OVERFLOW_LIMIT) ? true : false;
    uint16_t key_size = key_overflow_flag ? sizeof(uint32_t) : key_size = record.key.size() * sizeof(char);
    uint32_t key_overfow_page_id = key_overflow_flag ? core::Executor::overflow_page_creation_helper(record.key.data(), record.key.size()) : 0;

    bool value_overflow_flag = (record.val.size() * sizeof(char) > config::OVERFLOW_LIMIT) ? true : false;
    uint16_t value_size = value_overflow_flag ? sizeof(uint32_t) :  value_size = record.val.size() * sizeof(char);  
    uint32_t value_overfow_page_id = value_overflow_flag ? core::Executor::overflow_page_creation_helper(record.key.data(), record.key.size()) : 0;
    
    uint16_t record_size = sizeof(uint16_t)     // key_size field
                        + sizeof(uint8_t)       // key_overflow flag
                        + key_size              // key data or overflow page id
                        + sizeof(uint16_t)      // value_size field
                        + sizeof(uint8_t)       // value_overflow flag
                        + value_size;           // value data or overflow page id

    uint16_t full_data_size = record_size + sizeof(uint16_t); // record + slot entry

    //get the page_id with sufficient space for the record or create a new page
    uint32_t page_id;
    if (free_space_directory_.checkFreeSpace(full_data_size) == true){
        page_id = free_space_directory_.consumeFreeSpace(full_data_size);
    }else{
        page_id = buffer_pool_manager_.createPage();
        types::Frame* frame = buffer_pool_manager_.fetchPageMut(page_id);
        char* buffer = frame->buffer;

        types::SlottedPageHeader header = {
            page_id,
            types::PageType::DATA,
            0,
            config::SLOTTED_HEADER_SIZE,
            config::PAGE_SIZE,
            0
        };

        core::SlottedPageManager::serializeSlottedPageHeader(buffer, header);
    }

    types::Frame* frame = buffer_pool_manager_.fetchPageMut(page_id);
    char* buffer = frame->buffer;

    uint16_t offset = core::SlottedPageManager::writeTuple(frame, record, key_overfow_page_id, value_overfow_page_id);

    page_index_.insert(record.key, {page_id, offset});
};

uint32_t core::Executor::overflow_page_creation_helper(const char* data, const size_t len){
    uint16_t curr_data_len = std::min((size_t)(config::PAGE_SIZE - config::OVERFLOW_HEADER_SIZE), len);
    uint8_t last_page = (curr_data_len == len);
    uint32_t next_page_id = last_page ? 0 : overflow_page_creation_helper(data + curr_data_len, len - curr_data_len);

    uint32_t free_page_id = free_space_directory_.checkFreePage() == true ? free_space_directory_.consumeFreePage() : buffer_pool_manager_.createPage();
    types::Frame* free_frame = buffer_pool_manager_.fetchPageMut(free_page_id);

    types::OverflowPageHeader header = {
        free_page_id,
        next_page_id,
        curr_data_len,
        types::PageType::OVERFLOW_PAGE,
        last_page
    };

    core::OverflowPageManager::writeData(free_frame, header, data);

    return free_page_id;
}