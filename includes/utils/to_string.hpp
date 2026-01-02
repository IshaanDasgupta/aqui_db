#pragma once
#include <string>
#include <client/token.hpp>
#include <core/types.hpp>

namespace utils {

std::string to_string(const core::Field& f);
std::string to_string(const core::FieldSchema t);

std::string to_string(const client::Token& t);
std::string to_string(const client::TokenType t);

}
