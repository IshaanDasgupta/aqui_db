#pragma once

#include "types/types.hpp"
#include "core/buffer_pool_manager.hpp"

namespace core{

class OverflowPageManager{
public:
    // writes the overflow page header and data into the given buffer
    static uint32_t createOverflowPage(
        core::FreeSpaceDirectory free_space_directory,
        core::PageDirectory page_directory,
        core::Buffer_Pool_Manager buffer_pool_manger,
        char* data,
        uint16_t len
    );
    static void serializeOverflowPageHeader(char (&buffer)[config::OVERFLOW_HEADER_SIZE], const types::OverflowPageHeader& header);
    static types::OverflowPageHeader deserializeOverflowPageHeader(const char* buffer);
    static std::string getOverflowValue(core::Buffer_Pool_Manager& buffer_pool_manger, const uint32_t page_id);

private:
    OverflowPageManager();
};

}