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
    // int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int Max_fd = serverSocket + 1;
    fd_set fr, fw, fe;

    FD_ZERO(&fr);
    FD_ZERO(&fw);
    FD_ZERO(&fe);
    FD_SET(serverSocket, &fr);
    FD_SET(serverSocket, &fe);
    while (1)
    {
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int count = 0;
        for (int i = 0; i < Max_fd; ++i)
        {
            if (FD_ISSET(i, &fr))
            {
                ++count;
            }
        }
        std::cout << "Before select call: " << count << std::endl;
        int nRet = select(Max_fd, &fr, &fw, &fe, &tv);
        if (nRet > 0)
        {
            // when someone connecta o communicates with message owner
            // a dedicated connection
        }
        else if (nRet == 0)
        {
            std::cout << "Nothing on port " << port << std::endl;
        }
        else
        {
            std::cout << "select fail" << port;
            exit(EXIT_FAILURE);
        }
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
*/
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
        for (unsigned i = 0; i < clientSockets.size(); ++i)
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
