#pragma once

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

class IRCServer
{
private:
    int serverSocket;
    int port;
    std::string password;
    std::vector<int> clientSockets;
    // IRCServer();
public:
    IRCServer(int portNum, const std::string &pwd);
    void acceptConnections();
    void handleCommunication(int clientSocket);
    ~IRCServer();
};

/*
IRCServer& IRCServer::operator=(const IRCServer& other) {
    if (this != &other) {
        // Perform member-wise assignment or deep copy if needed
        this->serverSocket = other.serverSocket;
        this->port = other.port;
        this->password = other.password;
        this->clientSockets = other.clientSockets; // If the vector requires deep copy
    }
    return *this;
}

IRCServer(const IRCServer& other) : port(other.port), password(other.password) {
        // Copy other necessary member variables
        // Deep copy clientSockets
        for (size_t i = 0; i < other.clientSockets.size(); ++i) {
            this->clientSockets.push_back(other.clientSockets[i]);
        }
    }

*/