#pragma once 

#include "core/buffer_pool_manager.hpp"

namespace core
{
 
class BucketDirectoryPageManager{
public:
    static uint32_t getBucketPageId(core::Buffer_Pool_Manager& buffer_pool_manager, uint32_t init_page_id, uint32_t bit_pat);
    static void updatePageIdMatchingSuffix(core::Buffer_Pool_Manager& buffer_pool_manager, uint32_t init_page_id, uint32_t bit_pat, uint16_t local_depth, uint32_t tgt_bucket_page_id);
    static void doubleDirectory(core::Buffer_Pool_Manager& buffer_pool_manager, uint32_t init_page_id);
    static void serializeBucketDirectoryPageHeader(char (&buffer)[config::BUCKETDIRECTORY_PAGE_HEADER_SIZE], const types::BucketDirectoryPageHeader& header);
    static types::BucketDirectoryPageHeader deserializeBucketDirectoryPageHeader(const char* buffer);
private:
    BucketDirectoryPageManager();
};

}