#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9000);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    std::string password = "4";

    int ret = send(sockfd, password.c_str(), password.length(), 0);
    if (ret < 0) {
        std::cerr << "Error sending password" << std::endl;
        close(sockfd);
        return -1;
    }

    
    char serverResponse[255];
    memset(serverResponse, 0, sizeof(serverResponse));
    recv(sockfd, serverResponse, sizeof(serverResponse), 0);
    std::cout << "Server response: " << serverResponse << std::endl;

    close(sockfd);
    return 0;
}
