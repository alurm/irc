#include "Utils.hpp"
#include <stdio.h>
int countParams(char **params) {
    int count = 0;
    while (params[count]) {
        count++;
    }
    return count;
}


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


bool endsWithCRLF(const std::string& str) {
    size_t length = str.length();
    if (length >= 2) {
        std::string lastTwoChars = str.substr(length - 2);
        return (lastTwoChars == "\r\n" || lastTwoChars == "\n");
    }
    return false;
}
