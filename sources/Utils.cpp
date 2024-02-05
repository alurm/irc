#include "Utils.hpp"
#include <stdio.h>

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');

    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    if (last == std::string::npos) {
        return "";
    }
    return str.substr(first, (last - first + 1));
}

bool is_valid_args(std::string port, std::string password)
{
    int port_n = std::atoi(port.c_str());

    if (password.empty() || password.length() < 8)
    {
        std::cout << "Error: password must contain at least 8 characters!" << std::endl;
        return false;
    }
    
    if (port.empty())
    {
        std::cout << "Error: empty port!" << std::endl;
        return false;
    }

    for (size_t i = 0; i < port.size(); ++i)
    {
        if (!std::isdigit(port[i]))
        {
            std::cout << "Error: port cant contain non numeric characters!" << std::endl;
            return false;
        }
    }

    if (port_n <= 0 || port_n > 65535)
    {
        std::cout << "Error: port must be from  1 to 65535!" << std::endl;
        return false;
    }
    
    return true;
}