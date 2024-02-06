#ifndef UTILS_HPP
#define UTILS_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Parser.hpp"

struct Server;

std::string trim(const std::string& str);
bool is_valid_args(std::string port, std::string password);
#endif
