#include "core/pages/overflow_page.hpp"


uint32_t core::OverflowPageManager::writeData(
        core::FreeSpaceDirectory& free_space_directory,
        core::Buffer_Pool_Manager& buffer_pool_manager,
        const char* data,
        const size_t len
    ){
    uint16_t curr_data_len = std::min((size_t)(config::PAGE_SIZE - config::OVERFLOW_HEADER_SIZE), len);
    uint8_t last_page = (curr_data_len == len);
    uint32_t next_page_id = last_page ? 0 : writeData(
                                                free_space_directory,
                                                buffer_pool_manager,
                                                data + curr_data_len, 
                                                len - curr_data_len
                                            );
    uint32_t free_page_id = free_space_directory.checkFreePage() == true ? free_space_directory.consumeFreePage() : buffer_pool_manager.createPage();
    types::Frame* free_frame = buffer_pool_manager.fetchPageMut(free_page_id);
    char* buffer = free_frame->buffer;

    types::OverflowPageHeader header = {
        free_page_id,
        next_page_id,
        curr_data_len,
        types::PageType::OVERFLOW_PAGE,
        last_page
    };

    // serialize the header and write it
    char headerBuffer[config::OVERFLOW_HEADER_SIZE];
    serializeOverflowPageHeader(headerBuffer, header);
    std::memcpy(buffer, headerBuffer, config::OVERFLOW_HEADER_SIZE);

    // write the data
    std::memcpy(buffer + config::OVERFLOW_HEADER_SIZE, data, curr_data_len);
    return free_page_id;
};

void core::OverflowPageManager::serializeOverflowPageHeader(char (&buffer)[config::OVERFLOW_HEADER_SIZE], const types::OverflowPageHeader& header){
    uint16_t pos = 0;

    memcpy(buffer + pos, &header.page_id, sizeof(header.page_id));
    pos += sizeof(header.page_id);
    memcpy(buffer + pos, &header.next_page_id, sizeof(header.next_page_id));
    pos += sizeof(header.next_page_id);
    memcpy(buffer + pos, &header.data_bytes, sizeof(header.data_bytes));
    pos += sizeof(header.data_bytes);
    memcpy(buffer + pos, &header.page_type, sizeof(header.page_type));
    pos += sizeof(header.page_type);
    memcpy(buffer + pos, &header.last_page, sizeof(header.last_page));

    return;
};

types::OverflowPageHeader core::OverflowPageManager::deserializeOverflowPageHeader(const char* buffer) {
    types::OverflowPageHeader header = types::OverflowPageHeader{};

    uint16_t pos = 0;

    memcpy(&header.page_id, buffer + pos, sizeof(header.page_id));
    pos += sizeof(header.page_id);
    memcpy(&header.next_page_id, buffer + pos, sizeof(header.next_page_id));
    pos += sizeof(header.next_page_id);
    memcpy(&header.data_bytes, buffer + pos, sizeof(header.data_bytes));
    pos += sizeof(header.data_bytes);
    memcpy(&header.page_type, buffer + pos, sizeof(header.page_type));
    pos += sizeof(header.page_type);
    memcpy(&header.last_page, buffer + pos, sizeof(header.last_page));

    return header;
};

std::string core::OverflowPageManager::getData(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id) {
    const types::Frame* frame = buffer_pool_manager.fetchPage(page_id);
    char* buffer = frame->buffer;

    std::string val = "";
    types::OverflowPageHeader header = deserializeOverflowPageHeader(buffer);

    const char* data_ptr = buffer + config::OVERFLOW_HEADER_SIZE;
    std::string curr_val(data_ptr, header.data_bytes);
    val.append(curr_val);

    if (!header.last_page){
        std::string rest = getData(buffer_pool_manager, page_id);
        val.append(rest);
    }

    return val;
};