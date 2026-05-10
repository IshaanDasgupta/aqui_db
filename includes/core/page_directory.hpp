#pragma once

#include "types/types.hpp"

namespace core
{

class PageDirectory{
public:
    types::FreePage createPage();
private:
};

}
