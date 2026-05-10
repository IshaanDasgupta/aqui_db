#pragma once
#include <stdexcept>
#include <cstdint>
#include <string>

namespace utils
{

template<typename T> T safeRead(const char* buffer, uint16_t& pos, const char* logicalEnd) {
    if (buffer + pos + sizeof(T) > logicalEnd) {
        throw std::runtime_error("Trying to read illegal memory");
    }

    T value;
    std::memcpy(&value, buffer + pos, sizeof(T));
    pos += sizeof(T);
    return value;
};

template<typename T> void safeWrite(char* buffer, uint16_t& pos, const T* data, const uint16_t data_size, const char* logicalEnd) {
    if (buffer + pos + data_size > logicalEnd) {
        throw std::runtime_error("Trying to write to illegal memory");
    }

    std::memcpy(buffer + pos, data, data_size);
    pos += data_size;

    return;
};

template<typename T> void safeBackWrite(char* buffer, uint16_t& pos, const T* data, const uint16_t data_size, const char* logicalBackEnd) {
    char* writeStart = buffer + pos - data_size;
    if (writeStart < logicalBackEnd) {
        throw std::runtime_error("Trying to write to illegal memory");
    }

    std::memcpy(writeStart, data, data_size);
    pos -= data_size;

    return;
};


void safeRead(const char* buffer, uint16_t& pos, std::string& container, const uint16_t data_size, const char* logicalEnd) {
    if (buffer + pos + data_size > logicalEnd) {
        throw std::runtime_error("Trying to read illegal memory");
    }

    container.assign(buffer + pos, data_size);
    pos += data_size;
    return;
};

}
