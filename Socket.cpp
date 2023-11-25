// #include "Socket.hpp"

// Socket::Socket() : socket_id(-1) {
//     create_socket();
// }

// Socket:: Socket(const Socket& copy) : socket_id(copy.socket_id){
// }

// Socket& Socket::operator=(const Socket& other) {
//     if (this == &other) {
//         return *this;
//     }
//     socket_id = other.socket_id;
//     return *this;
// }

// Socket::~Socket() {
//     if (socket_id != -1) {
//         close(socket_id);
//     }
// }

// int Socket::create_socket() {
//     int sockfd;
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
//     if (sockfd < 0) {
//         throw std::runtime_error("Error creating socket");
//     }

//     int optval = 1;
//     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
//         close(sockfd);
//         throw std::runtime_error("Error setting socket option");
//     }

//     socket_id = sockfd;
//     return 0;
// }