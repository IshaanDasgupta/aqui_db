#pragma once

#include "utils/config.hpp"
#include<variant>
#include<string>
#include<optional>
#include<vector>


namespace types
{

struct PutQuery {
    std::string key;
    std::string val;
};

struct GetQuery {
    std::string key;
};

struct GetAllQuery {};

struct DelQuery {
    std::string key;
};

using Query = std::variant<PutQuery, GetQuery, GetAllQuery, DelQuery>;

struct Tuple{
    std::string key;
    std::string val;
};

using QueryRes = std::variant<
    std::monostate,          // PUT / DEL
    Tuple,                   // GET
    std::vector<Tuple>,      // GET_ALL
    std::string              // ERROR
>;

struct StoreHeader{
};

struct Frame
{
    bool is_dirty;
    char (&buffer)[config::PAGE_SIZE];
};

enum class PageType : uint16_t {
    INVALID         = 0,
    DATA            = 1,
    INDEX           = 2,
    OVERFLOW_PAGE   = 3
};

struct SlottedPageHeader{
    PageType page_type; // 2 bytes
    uint32_t page_id;
    uint32_t tuple_count;
    uint32_t slot_end_offset;
    uint32_t record_start_offset;
    uint32_t fragmented_bytes;
};

struct SlottedPage {
    char (&buffer)[config::PAGE_SIZE];

    const SlottedPageHeader* header() const {
        return reinterpret_cast<const SlottedPageHeader*>(buffer);
    }

    SlottedPageHeader* header() {
        return reinterpret_cast<SlottedPageHeader*>(buffer);
    }
};

struct OverflowPageHeader{
    uint32_t page_id;
    uint32_t next_page_id;
    uint16_t data_bytes;
    PageType page_type; // 2 bytes
    uint8_t last_page;
};

struct OverflowPage {
    char (&buffer)[config::PAGE_SIZE];

    const OverflowPageHeader* header() const {
        return reinterpret_cast<const OverflowPageHeader*>(buffer);
    }

    OverflowPageHeader* header()  {
        return reinterpret_cast<OverflowPageHeader*>(buffer);
    }
};

struct FreePage {
    uint32_t page_id;
    uint16_t offset;
}l

}
