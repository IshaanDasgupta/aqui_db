#pragma once

#include <exception>
#include <string>

namespace client {

class ClientException : public std::exception {
public:
    explicit ClientException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
private:
    std::string message;
};

class InvalidCommand : public ClientException {
public:
    explicit InvalidCommand(const std::string& command)
        : ClientException("Invalid command '" + command + "'") {}
};

}
