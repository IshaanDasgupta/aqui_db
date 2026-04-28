#pragma once

#include "utils/config.hpp"
#include <fstream>
#include <string>
#include <cstdint>

namespace core
{
class Page_Manager {
public:
    Page_Manager();
    ~Page_Manager();
    
    // reads a single page (in raw binary) from disk
    void read_page(const uint32_t offset, char (&buffer)[config::PAGE_SIZE]);
    // writes a single page (in raw binary) to disk
    void write_page(const uint32_t offset, const char (&buffer)[config::PAGE_SIZE]);
private:
    std::fstream file_;
};   
} // namespace core

