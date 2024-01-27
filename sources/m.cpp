// #include <iostream>
// #include <variant>

// template<typename T, typename E>
// struct Expected {
//     std::variant<T, E> result;

//     bool hasValue() const { return result.index() == 0; }
// };

// struct Error {
//     int code;
// };

// Expected<int, Error> divide(int a, int b) {
//     if (b == 0) {
//         return Expected<int, Error>{ Error{ 101 } };
//     }
//     return Expected<int, Error>{ a / b };
// }

// int main() {
//     auto result = divide(10, 2);
//     if (result.hasValue()) {
//         std::cout << "Result: " << std::get<int>(result.result) << std::endl;
//     } else {
//         std::cout << "Error Code: " << std::get<Error>(result.result).code << std::endl;
//     }

//     return 0;
// }

#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>

#include "Parser.hpp"

int main() {
    const char *ipAddress = "192.168.1.1";
    struct in_addr ipv4Addr;

    // Attempt to convert the IPv4 address to binary format
    int result = inet_pton(999, ipAddress, &ipv4Addr);

    if (result == 1) {
        printf("IPv4 address in binary: 0x%08x\n", ipv4Addr.s_addr);
    } else if (result == 0) {
        printf("Invalid IPv4 address\n");
    } else if (result == -1) {
        printf("Unsupported address family. Error: %s\n", strerror(errno));
    }

    return 0;
}


/*
    ** IPv4 only (see sockaddr_in6 for IPv6) **
    struct sockaddr_in {
        sa_family_t    sin_family;
        in_port_t      sin_port;
        struct in_addr sin_addr;
    };
    struct in_addr {
        uint32_t       s_addr;//network byte order integer
    };
*/