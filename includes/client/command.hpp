#pragma once

#include <tl/expected.hpp>
#include <string>

namespace client {

class ClientException;
class InvalidCommand; 

enum class CommandType {
    create,
    insert,
    get,
    get_all
};

std::string to_string(CommandType command);

tl::expected<CommandType, ClientException>
to_command_enum(const std::string& command);

};