#include "client/command.hpp"
#include "client/exception.hpp"

namespace client {

std::string to_string(CommandType command) {
    switch (command) {
        case CommandType::create: return "create";
        case CommandType::get: return "get";
        case CommandType::get_all: return "get_all";
        case CommandType::insert: return "insert";
    }
    return "unknown";
}

tl::expected<CommandType, ClientException>
to_command_enum(const std::string& command) {
    if (command == "create") return CommandType::create;
    if (command == "get") return CommandType::get;
    if (command == "get_all") return CommandType::get_all;
    if (command == "insert") return CommandType::insert;

    return tl::unexpected(InvalidCommand(command));
}

}
