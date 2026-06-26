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
    static void writeData(
        types::Frame* frame,
        types::OverflowPageHeader header,
        const char* data
    );
    static void serializeOverflowPageHeader(char* buffer, const types::OverflowPageHeader& header);
    static types::OverflowPageHeader deserializeOverflowPageHeader(const char* buffer);

private:
    OverflowPageManager();
};

}