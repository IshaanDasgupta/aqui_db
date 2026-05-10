#include "core/slotted_page.hpp"
#include "core/buffer_pool_manager.hpp"
#include "core/overflow_page.hpp"
#include "types/types.hpp"
#include "utils/memory.hpp"

//TODO: verify sizes
types::Tuple core::SlottedPageManager::readTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, const uint16_t offset){
    const types::Frame* frame = buffer_pool_manager.fetchPage(page_id);
    char* buffer = frame->buffer;

    const types::SlottedPageHeader header = deserializeSlottedPageHeader(buffer);    

    if(offset < config::SLOTTED_HEADER_SIZE || offset >= header.record_start_offset){
        throw std::runtime_error("Illegal offset");
    }

    uint16_t pos = offset;
    char* logicalSlotEnd = buffer + header.record_start_offset;

    uint16_t key_size = utils::safeRead<uint16_t>(buffer, pos, logicalSlotEnd);
    uint8_t key_overflow = utils::safeRead<uint8_t>(buffer, pos, logicalSlotEnd);

    std::string key;
    if (key_overflow == true){
        uint32_t curr_overflow_page_id = utils::safeRead<uint32_t>(buffer, pos, logicalSlotEnd);
        key = core::OverflowPageManager::getData(buffer_pool_manager, curr_overflow_page_id);
    }else{
        utils::safeRead(buffer, pos, key, key_size, logicalSlotEnd);
    }

    uint16_t value_size = utils::safeRead<uint16_t>(buffer, pos, logicalSlotEnd);
    uint8_t value_overflow = utils::safeRead<uint8_t>(buffer, pos, logicalSlotEnd);

    std::string value;
    if (value_overflow == true){
        uint32_t curr_overflow_page_id = utils::safeRead<uint32_t>(buffer, pos, logicalSlotEnd);
        value = core::OverflowPageManager::getData(buffer_pool_manager, curr_overflow_page_id);
    }else{
        utils::safeRead(buffer, pos, value, value_size, logicalSlotEnd);
    }

    return types::Tuple{key, value};
 
};

uint32_t core::SlottedPageManager::writeTuple(
        core::FreeSpaceDirectory free_space_directory,
        core::Buffer_Pool_Manager& buffer_pool_manager,
        const types::Tuple& tuple
    ){
    // compute key, value and meta-data sizes
    uint16_t key_size;
    uint8_t key_overfow;
    std::string key;
    uint32_t key_overfow_page_id;
    if (tuple.key.size() * sizeof(char) > config::OVERFLOW_LIMIT){
        key_size = sizeof(key_overfow_page_id);
        key_overfow = 1;
    }else{
        key_size = tuple.key.size() * sizeof(char);
        key_overfow = 0;
        key = tuple.key;
    }

    uint16_t value_size;
    uint8_t value_overfow;
    std::string value;    
    uint32_t value_overfow_page_id;
    if (tuple.val.size() * sizeof(char) > config::OVERFLOW_LIMIT){
        value_size = sizeof(value_overfow_page_id);
        value_overfow = 1;
    }else{
        value_size = tuple.val.size() * sizeof(char);
        value_overfow = 0;
        value = tuple.val;
    }

    uint16_t record_size = sizeof(key_size)     // key_size field
                        + sizeof(key_overfow)   // key_overflow flag
                        + key_size              // key data or overflow page id
                        + sizeof(value_size)    // value_size field
                        + sizeof(value_overfow) // value_overflow flag
                        + value_size;           // value data or overflow page id

    uint16_t tuple_size = record_size + sizeof(uint16_t); // record + slot entry


    //get the page_id with sufficient space for the tuple or create a new page
    uint32_t page_id = free_space_directory.checkFreeSpace(tuple_size) == true ? free_space_directory.consumeFreeSpace(tuple_size) : buffer_pool_manager.createPage();
    types::Frame* frame = buffer_pool_manager.fetchPageMut(page_id);
    char* buffer = frame->buffer;

    types::SlottedPageHeader header = deserializeSlottedPageHeader(buffer);    

    // check whether the tuple can fit into the current page
    uint16_t continuous_free_space = header.record_start_offset - header.slot_end_offset;
    uint16_t total_free_space = continuous_free_space + header.fragmented_bytes;

    if (tuple_size > total_free_space){
        throw std::runtime_error("Could not fit tuple into given page");
        return;
    }

    if (tuple_size > continuous_free_space){
        //TODO: implement compaction;
    }

    // create overflow pages for key and value if required
    key_overfow_page_id = key_overfow ? core::OverflowPageManager::writeData(free_space_directory, buffer_pool_manager, tuple.key.data(), tuple.key.size()) : 0;
    value_overfow_page_id = value_overfow ? core::OverflowPageManager::writeData(free_space_directory, buffer_pool_manager, tuple.val.data(), tuple.val.size()) : 0;
    
    // start writing record backwards
    uint16_t pos = header.record_start_offset;
    char* logicalBackEnd = buffer + header.slot_end_offset + sizeof(pos);

    char* value_data = value_overfow
        ? reinterpret_cast<char*>(&value_overfow_page_id)
        : value.data();
    utils::safeBackWrite(buffer, pos, value_data, value_size, logicalBackEnd);

    utils::safeBackWrite(buffer, pos, &value_overfow, sizeof(value_overfow), logicalBackEnd);
    utils::safeBackWrite(buffer, pos, &value_size, sizeof(value_size), logicalBackEnd);

    char* logicalBackEnd = buffer + header.slot_end_offset;

    char* key_data = key_overfow
        ? reinterpret_cast<char*>(&key_overfow_page_id)
        : key.data();
    utils::safeBackWrite(buffer, pos, key_data, key_size, logicalBackEnd);

    utils::safeBackWrite(buffer, pos, &key_overfow, sizeof(key_overfow), logicalBackEnd);
    utils::safeBackWrite(buffer, pos, &key_size, sizeof(key_size), logicalBackEnd);

    // write slot offset
    uint16_t slot_pos = header.slot_end_offset;
    utils::safeWrite(buffer, slot_pos, &pos, sizeof(pos), buffer + pos);

    // update header values
    header.record_start_offset = pos;
    header.slot_end_offset += sizeof(pos);
    header.tuple_count++;

    // mark the page dirty so that it gets flused to disk when it is evicted from the buffer pool
    frame->is_dirty = 1;

    // return the start of the currently written slot
    return header.slot_end_offset - sizeof(pos);    
}

uint32_t core::SlottedPageManager::deleteTuple(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id, const uint32_t offset){

};

void core::SlottedPageManager::serializeSlottedPageHeader(char (&buffer)[config::SLOTTED_HEADER_SIZE], const types::SlottedPageHeader& header){
    uint16_t pos = 0;

    memcpy(buffer+pos, &header.page_id, sizeof(header.page_id));
    pos += sizeof(header.page_id);
    memcpy(buffer+pos, &header.page_type, sizeof(header.page_type));
    pos += sizeof(header.page_type);
    memcpy(buffer+pos, &header.tuple_count, sizeof(header.tuple_count));
    pos += sizeof(header.tuple_count);
    memcpy(buffer+pos, &header.slot_end_offset, sizeof(header.slot_end_offset));
    pos += sizeof(header.slot_end_offset);
    memcpy(buffer+pos, &header.record_start_offset, sizeof(header.record_start_offset));
    pos += sizeof(header.record_start_offset);
    memcpy(buffer+pos, &header.fragmented_bytes, sizeof(header.fragmented_bytes));

    return;
};

types::SlottedPageHeader core::SlottedPageManager::deserializeSlottedPageHeader(const char* buffer){
    uint16_t pos = 0;
    types::SlottedPageHeader header = {};

    memcpy(&header.page_id, buffer+pos, sizeof(header.page_id));
    pos += sizeof(header.page_id);
    memcpy(&header.page_type, buffer+pos, sizeof(header.page_type));
    pos += sizeof(header.page_type);
    memcpy(&header.tuple_count, buffer+pos, sizeof(header.tuple_count));
    pos += sizeof(header.tuple_count);
    memcpy(&header.slot_end_offset, buffer+pos, sizeof(header.slot_end_offset));
    pos += sizeof(header.slot_end_offset);
    memcpy(&header.record_start_offset, buffer+pos, sizeof(header.record_start_offset));
    pos += sizeof(header.record_start_offset);
    memcpy(&header.fragmented_bytes, buffer+pos, sizeof(header.fragmented_bytes));

    return header;
};