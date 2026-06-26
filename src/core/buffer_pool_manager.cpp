#include "core/buffer_pool_manager.hpp"
#include "utils/config.hpp"

core::Buffer_Pool_Manager::Buffer_Pool_Manager(core::PageDirectory& page_directory)
    : page_directory_(page_directory),
      page_manger_(),
      page_buffer_(config::BUFFER_POOL_SIZE)
{
}

const types::Frame* core::Buffer_Pool_Manager::fetchPage(const uint32_t page_id)
{
    if (page_id_to_idx_.find(page_id) == page_id_to_idx_.end()){
        uint16_t free_index = getFreeFrameIndx();

        //TODO: better way to do page offsets
        uint32_t page_offset = config::STORE_HEADER_SIZE + (page_id * config::PAGE_SIZE);

        page_buffer_[free_index].page_id = page_id;
        page_buffer_[free_index].is_dirty = 0;
        page_manger_.read_page(page_offset, page_buffer_[free_index].buffer);

        page_id_to_idx_[page_id] = free_index;

        return &(page_buffer_[free_index]);
    }

    return &(page_buffer_[page_id_to_idx_[page_id]]);
}

types::Frame *core::Buffer_Pool_Manager::fetchPageMut(const uint32_t page_id)
{
    if (page_id_to_idx_.find(page_id) == page_id_to_idx_.end()){
        uint16_t free_index = getFreeFrameIndx();

        //TODO: better way to do page offsets
        uint32_t page_offset = config::STORE_HEADER_SIZE + (page_id * config::PAGE_SIZE);

        page_buffer_[free_index].page_id = page_id;
        page_buffer_[free_index].is_dirty = 0;
        page_manger_.read_page(page_offset, page_buffer_[free_index].buffer);

        page_id_to_idx_[page_id] = free_index;

        return &(page_buffer_[free_index]);
    }

    return &(page_buffer_[page_id_to_idx_[page_id]]);
}

void core::Buffer_Pool_Manager::flushPage(const uint32_t page_id)
{
    if (page_id_to_idx_.find(page_id) == page_id_to_idx_.end()){
        throw std::runtime_error("Invalid page id while flusing page to disk");
    }

    uint32_t page_offset = config::STORE_HEADER_SIZE + (page_id * config::PAGE_SIZE);
    page_manger_.write_page(page_offset, page_buffer_[page_id_to_idx_[page_id]].buffer);

    return;
}

uint32_t core::Buffer_Pool_Manager::createPage()
{
    uint32_t new_page_id = page_directory_.getNextPageId();
    uint16_t free_indx = getFreeFrameIndx();

    page_buffer_[free_indx].page_id = new_page_id;
    page_buffer_[free_indx].is_dirty = 1;
    memset(page_buffer_[free_indx].buffer, 0 , sizeof(page_buffer_[free_indx].buffer));

    page_directory_.incrementTotalPages();

    return new_page_id;
}

uint16_t core::Buffer_Pool_Manager::getFreeFrameIndx()
{
    if (page_id_to_idx_.size() < config::BUFFER_POOL_SIZE){
        return page_id_to_idx_.size();
    }

    //TODO: implement eviction policy (for now just evicting the first frame)
    uint16_t eviction_indx = 0;
    if (page_buffer_[eviction_indx].is_dirty == 1){
        flushPage(page_buffer_[eviction_indx].page_id);
    }

    page_id_to_idx_.erase(page_buffer_[eviction_indx].page_id);

    return eviction_indx;
}
