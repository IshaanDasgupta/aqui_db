#include "core/page_manager.hpp"
#include "utils/config.hpp"

core::PageManager::PageManager(){
    file_.open(config::db_path, std::ios::in | std::ios::out | std::ios::binary);

    if (!file_.is_open()) {
        file_.open(config::db_path, std::ios::out | std::ios::binary);
        file_.close();
        file_.open(config::db_path, std::ios::in | std::ios::out | std::ios::binary);
    }

    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open file");
    }
};

core::PageManager::~PageManager(){
    if (file_.is_open()) {
        file_.close();
    }
};

void core::PageManager::read_page(const uint32_t offset, char (&buffer)[config::PAGE_SIZE]){
    file_.seekg(offset);
    file_.read(buffer, sizeof(buffer));

    if (file_.gcount() != config::PAGE_SIZE){
        throw std::runtime_error("Trying to read garbage values");
    }
}

void core::PageManager::write_page(const uint32_t offset, const char (&buffer)[config::PAGE_SIZE]){
    file_.seekp(offset);
    file_.write(buffer, sizeof(buffer));
    return;
}