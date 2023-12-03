#include "IRCServer.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>

IRCServer::IRCServer(int portNum, const std::string &pwd) : port(portNum), password(pwd)
{
    initializeServer();
    startListening();
}

void IRCServer::fterror(char *str)
{
    if (s > 2)
        close(s);
    write(2, str, strlen(str));
    exit(1);
}

void IRCServer::initializeServer()
{
    len = sizeof(cli);
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
    {
        fterror("Fatal error\n");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // servaddr.sin_addr.s_addr = htonl(2130706433);
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // INADDR_ANY
    memset(&servaddr.sin_zero, 0, 8);
    int optval = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
    {
        handleError("Error: setsockopt failed");
    }

    if (bind(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        fterror("Error: binding socket");
    }

    if (listen(s, SOMAXCONN) < 0)
    {
        fterror("Error: listening on socket");
    }

    showServerInfo();
    setSocketNonBlocking(s);
}

void IRCServer::startListening()
{
    fdMax = s;
    FD_ZERO(&action);
    FD_SET(s, &action);
    idNext = 0;
    while (true)
    {

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        ready = fd = action;
        if (select(fdMax + 1, &ready, &fd, 0, 0) < 0)
            continue;
        if (FD_ISSET(s, &ready))
        {
            sclient = accept(s, (struct sockaddr *)&cli, &len);
            if (sclient < 0)
                continue;

            FD_SET(sclient, &action);
            sprintf(buffer, "server: client %d just arrived\n", idNext);
            clients[sclient] = idNext++;
            char buff[255] = {0};
            FD_SET(sclient, &action);
            int bytesReceived = recv(sclient, buff, 255, 0);
            std::cout << "Recieved message is: " << buff << std::endl;
            if (bytesReceived > 0)
            {
                std::cout << "Received password from client: " << buff << std::endl;
                if (strcmp(buff, password.c_str()) == 0)
                {
                    send(sclient, "Password accepted. You are connected.\n", strlen("Password accepted. You are connected.\n"), 0);
                }
                else
                {
                    send(sclient, "Incorrect password. Connection denied.\n", strlen("Incorrect password. Connection denied.\n"), 0);
                    close(sclient);
                    continue;
                }
            }
            setSocketNonBlocking(sclient);
            sendAll(sclient);
            fdMax = sclient > fdMax ? sclient : fdMax;
            continue;
        }
        for (int i = 3; i <= fdMax; ++i)
        {
            if (FD_ISSET(i, &ready))
            {
                int res = 1;
                char msg[20000];
                bzero(&msg, sizeof(msg));
                while (res == 1 && msg[strlen(msg) - 1] != '\n')
                    res = recv(i, msg + strlen(msg), 1, 0);
                if (res <= 0)
                {
                    sprintf(buffer, "server: client %d just left\n", clients[i]);
                    FD_CLR(i, &action);
                    close(i);
                }
                else
                    sprintf(buffer, "client %d: %s", clients[i], msg);
                sendAll(i);
            }
        }
    }
}

void IRCServer::showServerInfo()
{
    struct sockaddr_in serverInfo;
    socklen_t len = sizeof(serverInfo);

    if (getsockname(s, (struct sockaddr *)&serverInfo, &len) == -1)
    {
        perror("getsockname");
    }
    else
    {
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(serverInfo.sin_addr), ipAddress, INET_ADDRSTRLEN);
        std::cout << "Server is bound to IP: " << ipAddress << ", Port: " << ntohs(serverInfo.sin_port) << std::endl;
    }
}

void IRCServer::setSocketNonBlocking(int socket)
{
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1)
    {
        handleError("Error: Unable to get socket flags");
    }

    flags |= O_NONBLOCK;
    if (fcntl(socket, F_SETFL, flags) == -1)
    {
        handleError("Error: Unable to set socket to non-blocking mode");
    }
}

void IRCServer::handleError(const std::string &message)
{
    std::cerr << message << ": " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
}

IRCServer::~IRCServer()
{
    close(s);

    for (size_t i = 0; i < sizeof(clients); ++i)
    {
        close(clients[i]);
    }
}

void IRCServer::sendAll(int n)
{
    for (int i = 3; i <= fdMax; ++i)
        if (FD_ISSET(i, &fd) && i != n)
            if (send(i, buffer, strlen(buffer), 0) < 0)
                fterror("Fatal error\n");
    bzero(&buffer, sizeof(buffer));
}