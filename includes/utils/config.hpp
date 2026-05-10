#pragma once

#include<cstdint>
#include<string>

namespace config {
    constexpr uint32_t VERSION = 1;
    constexpr const char* db_path = R"(C:\playground\aqui-db\data.aqui)";
    constexpr uint32_t MAGIC = 0x41515549; // "AQUI"
    constexpr uint16_t PAGE_SIZE = 4096; // 4KB
    constexpr uint32_t OVERFLOW_LIMIT = 1024; // 1KB
    constexpr uint32_t BUFFER_POOL_SIZE = 16; // 16 pages
    constexpr uint16_t STORE_HEADER_SIZE = 13; // 13 bytes
    constexpr uint16_t OVERFLOW_HEADER_SIZE = 13; // 13 bytes
    constexpr uint16_t SLOTTED_HEADER_SIZE = 13; // 13 bytes
}