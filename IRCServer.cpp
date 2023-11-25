#include "IRCServer.hpp"

IRCServer::IRCServer(int portNum, const std::string &pwd) : port(portNum), password(pwd)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        std::cerr << "Error: The socket not opened" << std::endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);
    // memset(&(srv.sin_zero), 0 ,8);
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Error: binding socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, SOMAXCONN) < 0)
    {
        std::cerr << "Error: listening on socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    //fr -> sockets from which i read
    //fw -> sockets when i wanna write
    //fe -> sockets that rthrow some errors
    // fd_set(fr, fw, fe)
}

void IRCServer::acceptConnections()
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddrLen = sizeof(clientAddress);

    while (true)
    {
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
        if (clientSocket < 0)
        {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }
        handleCommunication(clientSocket);
        // Handle new client connection
        // Validate password, manage IRC communication, etc.
        // Add clientSocket to clientSockets vector

        // Example: Here, you might read/write to the clientSocket for IRC communication
        // Assuming clientSockets is a vector of client socket descriptors

        // Start a thread or handle communication in a separate function
        // Example: std::thread communicationThread(&IRCServer::handleCommunication, this, clientSocket);
        // communicationThread.detach(); // Detach the thread to run independently
    }
}

void IRCServer::handleCommunication(int clientSocket)
{
    char buffer[1024];
    int bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
    {
        // Broadcast received message to all other clients
        for (unsigned  i = 0; i < clientSockets.size(); ++i)
        {
            if (clientSockets[i] != clientSocket)
            {
                send(clientSockets[i], buffer, bytesRead, 0);
            }
        }
    }

    // Handle client disconnection
    // Remove the clientSocket from the clientSockets vector and perform cleanup
}


IRCServer::~IRCServer()
{
    close(serverSocket);
    
    for (std::vector<int>::iterator it = clientSockets.begin(); it != clientSockets.end(); ++it)
    {
        int clientSock = *it;
        close(clientSock);
    }
}
