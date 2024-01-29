#ifndef UTILS_HPP
#define UTILS_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Parser.hpp"

struct Server;

// Buggy?
int countParams(char **params);
std::string trim(const std::string& str);

#endif
