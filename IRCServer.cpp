#include "IRCServer.hpp"

void IRCServer::ProcessTheNewRequest()
{
    // new connection request
    if (FD_ISSET(serverSocket, &f))
    {
        std::cout << "here" << std::endl;
        struct sockaddr_in clientAddress;
        socklen_t clientAddrLen = sizeof(clientAddress);
        int newClientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
        if (newClientSocket < 0)
        {
            // Handle error in accepting client
        }
        else
        {
            // Handle new client connection
            clientSockets.push_back(newClientSocket);
            const char *message = "Got the connection done\n";
            size_t messageLen = strlen(message);
            ssize_t bytesSent = send(newClientSocket, message, messageLen, 0);

            if (bytesSent == -1)
            {
                std::cerr << "Error sending data" << std::endl;
            }
            else
            {
                handleCommunication(newClientSocket);
                std::cout << "Data sent successfully" << std::endl;
            }
        }
    }
}

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
    // serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    serverAddress.sin_port = htons(port);
    // memset(&(srv.sin_zero), 0 ,8);

    /*blocking*/
    int flags = fcntl(serverSocket, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "Error: Unable to get socket flags" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    flags |= O_NONBLOCK;
    if (fcntl(serverSocket, F_SETFL, flags) == -1)
    {
        std::cerr << "Error: Unable to set socket to non-blocking mode" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    int optval = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        std::cerr << "Error: setsockopt failed" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cerr << "Error: setsockopt successed" << std::endl;
    }

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr << "Error: binding socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serverInfo;
    socklen_t len = sizeof(serverInfo);

    if (getsockname(serverSocket, (struct sockaddr *)&serverInfo, &len) == -1)
    {
        perror("getsockname");
    }
    else
    {
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(serverInfo.sin_addr), ipAddress, INET_ADDRSTRLEN);
        std::cout << "Server is bound to IP: " << ipAddress << ", Port: " << ntohs(serverInfo.sin_port) << std::endl;
    }

    if (listen(serverSocket, SOMAXCONN) < 0)
    {
        std::cerr << "Error: listening on socket" << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        int Max_fd = serverSocket + 1;
        FD_ZERO(&f);
        FD_SET(serverSocket, &f);
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int nRet = select(Max_fd, &f, nullptr, nullptr, &tv);
        if (nRet > 0)
        {
            ProcessTheNewRequest();
        }
        else if (nRet == 0)
        {
            std::cout << "Nothing on port " << port << std::endl;
        }
        else
        {
            std::cout << "select fail" << port;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
    }
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

    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0)
    {
        std::cout << "just press any key to see the message" << std::endl;
        // Broadcast received message to all other clients
        for (unsigned i = 0; i < clientSockets.size(); ++i)
        {
            if (clientSockets[i] != clientSocket)
            {
                send(clientSockets[i], buffer, bytesRead, 0);
                std::cout << std::endl
                          << buffer << std::endl;
            }
        }
    }
    // while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
    // {
    //     // Broadcast received message to all other clients
    //     for (unsigned i = 0; i < clientSockets.size(); ++i)
    //     {
    //         if (clientSockets[i] != clientSocket)
    //         {
    //             send(clientSockets[i], buffer, bytesRead, 0);
    //             std::cout << std::endl << buffer << std::endl;
    //         }
    //     }
    // }

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

// fd_set fr -> sockets from which i read
// fd_set fw -> sockets when i wanna write
// fd_set fe -> sockets that throw some errors
/*
    windows specific
    typedef struct fd_set {
        u_int  fd_count;
        SOCKET fd_array[FD_SETSIZE];
        //these are open file descriptors in kernel(
            whenever i create an file ist kernel object
        )
    } fd_set
    fd_count indicates the current number
    of sockets set in a particular fd_set.
    FD_SETSIZE maximum for sockets -> 64
    fd_set int n = 100;
    for (int nIndex = 0; nIndex < 64; nIndex++)
    {
        FD_SET(n, &fr)
        n++;
    }

    std::cout << std::endl < fr.fd_count << std::endl;

    for (int nIndex = 0; nIndex < 99; nIndex++)
    {
        std::cout << fr.fd_array[nIndex];
    }

     // fd_set my_set; // Create an fd_set
    // FD_ZERO(&my_set); // Initialize the set to empty

    // int n = 100;
    // for (int nIndex = 0; nIndex < 10240; nIndex++) {
    //     FD_SET(n, &my_set); // Add file descriptors to the set
    //     n++;
    // }

    // std::cout << "Number of file descriptors in the set: " << my_set.fds_bits[0] << std::endl;

    // std::cout << "File descriptors in the set: ";
    // for (int nIndex = 0; nIndex < FD_SETSIZE; nIndex++) {
    //     if (FD_ISSET(nIndex, &my_set)) {
    //         std::cout << nIndex << " ";
    //     }
    // }
    // std::cout << std::endl;

     u_long optval = 0;
    nRet = localsocket(serverSocket, FIOBIO, &optval);
    //every socket by default is blocking socket
    //blocking ---> FIOBIO
    //optval -> 0 -> bloclong
    if(nRet != 0)
    {
        std::cout << std::endl << "ioctlsocket call passed" << std::endl;
    }else {
        std::cout << std::endl << "ioctlsocket call failed" << std::endl;
    }
*/