#include "core/pages/bucket_directory_page.hpp"

uint32_t core::BucketDirectoryPageManager::getBucketPageId(core::Buffer_Pool_Manager &buffer_pool_manager, uint32_t init_page_id, uint32_t bit_pat){
    const types::Frame* frame = buffer_pool_manager.fetchPage(init_page_id);

    const char* buffer;
    types::BucketDirectoryPageHeader header;
    uint16_t curr_bucket_arr_size;

    buffer = frame->buffer;

    header = deserializeBucketDirectoryPageHeader(buffer);
    curr_bucket_arr_size = header.data_bytes/config::PAGE_ID_SIZE;

    while(bit_pat >= curr_bucket_arr_size){
        if (header.last_page == 1){
            throw std::runtime_error("Trying to access invalid bit pattern while accesing a key in Bucket Directory");
        }

        if (curr_bucket_arr_size == 0){
            throw std::runtime_error("Ran into a bucket_directory page with 0 data bytes during lookup causing a infinte loop");
        }

        bit_pat -= curr_bucket_arr_size;

        const types::Frame* next_frame = buffer_pool_manager.fetchPage(header.next_page_id);

        buffer = next_frame->buffer;
        header = deserializeBucketDirectoryPageHeader(buffer);
        curr_bucket_arr_size = header.data_bytes/config::PAGE_ID_SIZE;
    }

    uint16_t offset = config::BUCKETDIRECTORY_PAGE_HEADER_SIZE + (bit_pat*config::PAGE_ID_SIZE);
    uint32_t bucket_page_id;
    memcpy(&bucket_page_id, buffer + offset, config::PAGE_ID_SIZE);

    return bucket_page_id;
}

void core::BucketDirectoryPageManager::updatePageIdMatchingSuffix(core::Buffer_Pool_Manager &buffer_pool_manager, uint32_t init_page_id, uint32_t bit_pat, uint16_t local_depth, uint32_t tgt_bucket_page_id){
    uint32_t curr_indx = bit_pat;
    uint32_t incre_uint = (1 << local_depth);
    uint32_t prev_count = 0;

    char* buffer = buffer_pool_manager.fetchPageMut(init_page_id)->buffer;
    types::BucketDirectoryPageHeader header = deserializeBucketDirectoryPageHeader(buffer);
    uint32_t curr_count = header.data_bytes/config::PAGE_ID_SIZE;

    while(1){
        while(curr_indx < curr_count + prev_count){
            uint16_t offset = config::BUCKETDIRECTORY_PAGE_HEADER_SIZE + ((curr_indx - prev_count) * config::PAGE_ID_SIZE);
            memcpy(buffer + offset, &tgt_bucket_page_id, config::BUCKETDIRECTORY_PAGE_HEADER_SIZE);

            curr_indx += incre_uint;
        }

        if (header.last_page == 1)
            break;
        
        prev_count += curr_count;
        buffer = buffer_pool_manager.fetchPageMut(header.next_page_id)->buffer;
        header = deserializeBucketDirectoryPageHeader(buffer);
        curr_count = header.data_bytes/config::PAGE_ID_SIZE;
    }

    return;
}


// copying 4 bytes at a time is bad in terms of performance hence batch memcpy is used
void core::BucketDirectoryPageManager::doubleDirectory(core::Buffer_Pool_Manager &buffer_pool_manager, uint32_t init_page_id){
    types::Frame* frame = buffer_pool_manager.fetchPageMut(init_page_id);

    //preping variables for a 2-pointer approach
    char* src_buffer = frame->buffer;
    types::BucketDirectoryPageHeader src_header = deserializeBucketDirectoryPageHeader(src_buffer);
    uint16_t src_offset = config::BUCKETDIRECTORY_PAGE_HEADER_SIZE;

    uint16_t new_global_depth = src_header.global_depth + 1;
    //update the global_depth for all existing pages
    src_header.global_depth = new_global_depth;
    char header_buffer[config::BUCKETDIRECTORY_PAGE_HEADER_SIZE];
    serializeBucketDirectoryPageHeader(header_buffer, src_header);
    memcpy(src_buffer, header_buffer, config::BUCKETDIRECTORY_PAGE_HEADER_SIZE);

    uint32_t end_page_id;
    uint16_t end_offset;

    char* temp_buffer = frame->buffer;
    types::BucketDirectoryPageHeader temp_header = deserializeBucketDirectoryPageHeader(temp_buffer);


    while(temp_header.last_page == 0){
        temp_buffer = buffer_pool_manager.fetchPageMut(temp_header.next_page_id)->buffer;
        temp_header = deserializeBucketDirectoryPageHeader(temp_buffer);

        //update the global_depth for all existing pages
        temp_header.global_depth = new_global_depth;
        char header_buffer[config::BUCKETDIRECTORY_PAGE_HEADER_SIZE];
        serializeBucketDirectoryPageHeader(header_buffer, temp_header);
        memcpy(temp_buffer, header_buffer, config::BUCKETDIRECTORY_PAGE_HEADER_SIZE);
    }

    end_page_id = temp_header.page_id;
    end_offset = config::BUCKETDIRECTORY_PAGE_HEADER_SIZE + temp_header.data_bytes;

    char* tgt_buffer = temp_buffer;
    types::BucketDirectoryPageHeader tgt_header = temp_header;
    uint16_t tgt_offset = config::BUCKETDIRECTORY_PAGE_HEADER_SIZE + temp_header.data_bytes;

    //running src and tgt pointers
    while(1){
        //as we want to maximise batch writes, we take the longest bytes possible considering both src and tgt
        //src would have a diffrent 'end' if the curr page it's on is the same as the original end_page
        uint16_t src_limit = src_header.page_id !=  end_page_id ? config::BUCKETDIRECTORY_PAGE_HEADER_SIZE + src_header.data_bytes : end_offset;
        uint16_t tgt_limit = config::PAGE_SIZE;

        //these would be the actual usable bytes as the remaining bytes would never be of use as we would not be able to fit a full page_id there
        uint16_t bytes_in_src = config::PAGE_ID_SIZE * ((src_limit  - src_offset) / config::PAGE_ID_SIZE);
        uint16_t bytes_in_tgt = config::PAGE_ID_SIZE * ((tgt_limit - tgt_offset) / config::PAGE_ID_SIZE);

        uint16_t bytes_taken = std::min(bytes_in_src, bytes_in_tgt);

        if (bytes_taken == 0)
            throw std::runtime_error("Ran into bytes_taken being 0 during doubling the bucket directory causing infinte loop");

        memcpy(tgt_buffer + tgt_offset, src_buffer + src_offset, bytes_taken);
        src_offset += bytes_taken;
        tgt_offset += bytes_taken;

        tgt_header.data_bytes += bytes_taken;

        if (src_limit - src_offset < config::PAGE_ID_SIZE){
            //if with the above condition, the below condition is met them, we would have copied all the original data
            if (src_header.page_id == end_page_id)
                break;
            
            //move src to the next page
            src_buffer = buffer_pool_manager.fetchPageMut(src_header.next_page_id)->buffer;
            src_header = deserializeBucketDirectoryPageHeader(src_buffer);
            src_offset = config::BUCKETDIRECTORY_PAGE_HEADER_SIZE;
        }

        if (tgt_limit - tgt_offset < config::PAGE_ID_SIZE){
            //provision a new page
            uint32_t next_page_id = buffer_pool_manager.createPage();

            //update the next_page pointer and last_page explicitly and wrtie the new data_bytes present in the header (updated during memcpy itself)
            //no need to update global_depth as all existing pages have updated global_depth and the new pages are created with the updated global_depth by default
            tgt_header.next_page_id = next_page_id;
            tgt_header.last_page = 0;
            char new_binary_header[config::BUCKETDIRECTORY_PAGE_HEADER_SIZE];   
            serializeBucketDirectoryPageHeader(new_binary_header, tgt_header);
            memcpy(tgt_buffer, new_binary_header, config::BUCKETDIRECTORY_PAGE_HEADER_SIZE);

            //move the tgt to the new page
            tgt_buffer = buffer_pool_manager.fetchPageMut(next_page_id)->buffer;
            tgt_header = {
                next_page_id,
                0,
                new_global_depth,
                0,
                types::PageType::BUCKET_DIRECTORY,
                1
            };
            char header_buffer[config::BUCKETDIRECTORY_PAGE_HEADER_SIZE];
            serializeBucketDirectoryPageHeader(header_buffer, tgt_header);
            memcpy(tgt_buffer, header_buffer, config::BUCKETDIRECTORY_PAGE_HEADER_SIZE);
            tgt_offset = config::BUCKETDIRECTORY_PAGE_HEADER_SIZE;

        }
    };

    //force flush the last bucket_directory page where tgt is pointing to
    serializeBucketDirectoryPageHeader(header_buffer, tgt_header);
    memcpy(tgt_buffer, header_buffer, config::BUCKETDIRECTORY_PAGE_HEADER_SIZE);

    return;
}

void core::BucketDirectoryPageManager::serializeBucketDirectoryPageHeader(char (&buffer)[config::BUCKETDIRECTORY_PAGE_HEADER_SIZE], const types::BucketDirectoryPageHeader &header)
{
    uint16_t pos = 0;

    memcpy(buffer + pos, &header.page_id, sizeof(header.page_id));
    pos += sizeof(header.page_id);
    memcpy(buffer + pos, &header.next_page_id, sizeof(header.next_page_id));
    pos += sizeof(header.next_page_id);
    memcpy(buffer + pos, &header.global_depth, sizeof(header.global_depth));
    pos += sizeof(header.global_depth);
    memcpy(buffer + pos, &header.data_bytes, sizeof(header.data_bytes));
    pos += sizeof(header.data_bytes);
    memcpy(buffer + pos, &header.page_type, sizeof(header.page_type));
    pos += sizeof(header.page_type);
    memcpy(buffer + pos, &header.last_page, sizeof(header.last_page));

    return;
}

types::BucketDirectoryPageHeader core::BucketDirectoryPageManager::deserializeBucketDirectoryPageHeader(const char * buffer){
    types::BucketDirectoryPageHeader header = types::BucketDirectoryPageHeader{};
    uint16_t pos = 0;

    memcpy(&header.page_id, buffer + pos, sizeof(header.page_id));
    pos += sizeof(header.page_id);
    memcpy(&header.next_page_id, buffer + pos, sizeof(header.next_page_id));
    pos += sizeof(header.next_page_id);
    memcpy(&header.global_depth, buffer + pos, sizeof(header.global_depth));
    pos += sizeof(header.global_depth);
    memcpy(&header.data_bytes, buffer + pos, sizeof(header.data_bytes));
    pos += sizeof(header.data_bytes);
    memcpy(&header.page_type, buffer + pos, sizeof(header.page_type));
    pos += sizeof(header.page_type);
    memcpy(&header.last_page, buffer + pos, sizeof(header.last_page));

    return header;
}