#include <iostream>
#include <string>
#include <core/page_manager.hpp>
#include <vector>

int main() {
    core::Page_Manager pm = core::Page_Manager();
    char page[config::PAGE_SIZE];

    std::vector<uint32_t> even(1024),odd(1024);
    for (int i=0 ; i<1024; i++){
        even[i] = 2*(i+1);
        odd[i] = 2*(i+1) - 1;

    }

    memcpy(page, even.data(), config::PAGE_SIZE);
    pm.write_page(0, page);

    memcpy(page, odd.data(), config::PAGE_SIZE);
    pm.write_page(config::PAGE_SIZE, page);

    std::vector<uint32_t> out(1024);

    pm.read_page(config::PAGE_SIZE, page);
    memcpy(out.data(), page, config::PAGE_SIZE);
    for (auto &i: out) 
        std::cout << i << " ";
    std::cout << "\n";

    pm.read_page(0, page);
    memcpy(out.data(), page, config::PAGE_SIZE);
    for (auto &i: out) 
        std::cout << i << " ";
    std::cout << "\n";

    return 0;
}
