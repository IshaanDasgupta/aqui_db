#pragma once

#include <variant>
#include <vector>
#include "types/types.hpp"
#include "core/executor.hpp"

namespace core{

class Planner{
public:
    Planner();
    // a switch statement which servers as an abstraction to handel all query types with a single function
    types::QueryRes execute(const types::Query& query);    
private:
    // internal defination of execution for each query type
    std::variant<std::string, std::monostate> execute_put(const types::PutQuery& put_query);
    std::variant<std::string, types::Tuple> execute_get(const types::GetQuery& get_query);
    std::variant<std::string, std::vector<types::Tuple>> execute_get_all(const types::GetAllQuery& get_all_query);
    std::variant<std::string, std::monostate> execute_del(const types::DelQuery& del_query);

    core::Executor executor;
};

}