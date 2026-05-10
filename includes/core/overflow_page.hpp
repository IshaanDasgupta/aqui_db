#pragma once

#include "types/types.hpp"
#include "core/free_space_directory.hpp"
#include "core/page_directory.hpp"
#include "core/buffer_pool_manager.hpp"

namespace core{

//TODO: deletion
class OverflowPageManager{
public:
    static std::string getData(core::Buffer_Pool_Manager& buffer_pool_manager, const uint32_t page_id);
    static uint32_t writeData(
        core::FreeSpaceDirectory& free_space_directory,
        core::Buffer_Pool_Manager& buffer_pool_manager,
        const char* data,
        const size_t len
    );
    static void serializeOverflowPageHeader(char (&buffer)[config::OVERFLOW_HEADER_SIZE], const types::OverflowPageHeader& header);
    static types::OverflowPageHeader deserializeOverflowPageHeader(const char* buffer);

private:
    OverflowPageManager();
};

}