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
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>

class IRCServer
{
private:
    int serverSocket;
    int port;
    std::string password;
    std::vector<int> clientSockets;
    fd_set f;
    // IRCServer();
public:
    IRCServer(int portNum, const std::string &pwd);
    void acceptConnections();
    void handleCommunication(int clientSocket);
    void ProcessTheNewRequest();
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


//NON BLOCKING
/*
• Only 1 poll() (or equivalent)
     can be used for handling 
    all these operations (read,
    write, but also listen, and so forth).
*/

/*
    Because you have to use non-blocking file descriptors, it is
    possible to use read/recv or write/send functions with no poll()
    (or equivalent), and your server wouldn’t be blocking.
    But it would consume more system resources.
    Thus, if you try to read/recv or write/send in any file descriptor
    without using poll() (or equivalent), your grade will be 0
*/

/*
Several IRC clients exist. You have to choose one of them as a reference. Your
reference client will be used during the evaluation process.
• Your reference client must be able to connect to your server without encountering
any error.
• Communication between client and server has to be done via TCP/IP (v4 or v6).
• Using your reference client with your server must be similar to using it with any
official IRC server. However, you only have to implement the following features:
◦ You must be able to authenticate, set a nickname, a username, join a channel,
send and receive private messages using your reference client.
◦ All the messages sent from one client to a channel have to be forwarded to
every other client that joined the channel.
◦ You must have operators and regular users.
◦ Then, you have to implement the commands that are specific to channel
operators:
∗ KICK - Eject a client from the channel
∗ INVITE - Invite a client to a channel
∗ TOPIC - Change or view the channel topic
∗ MODE - Change the channel’s mode:
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel
operators
· k: Set/remove the channel key (password)
· o: Give/take channel operator privilege
*/

/*
    Set/remove the user limit to channel
*/

/*
    For MacOS only
    Since MacOS doesn’t implement write() the same way as other Unix
    OSes, you are allowed to use fcntl().
    You must use file descriptors in non-blocking mode in order to get a
    behavior similar to the one of other Unix OSes.
    However, you are allowed to use fcntl() only as follows:
    fcntl(fd, F_SETFL, O_NONBLOCK);
    Any other flag is forbidden.
*/

/*
    Test example
    Verify absolutely every possible error and issue (receiving partial data, low bandwidth,
    and so forth).
    To ensure that your server correctly processes everything that you send to it, the
    following simple test using nc can be done:
    \$> nc 127.0.0.1 6667
    com^Dman^Dd
    \$>
    Use ctrl+D to send the command in several parts: ’com’, then ’man’, then ’d\n’.
    In order to process a command, you have to first aggregate the received packets in
    order to rebuild it.
*/

/*
    Bonus part
    Here are the extra features you can add to your IRC server so it looks even more like and
    actual IRC server:
    • Handle file transfer.
    • A bot.
    The bonus part will only be assessed if the mandatory part is
    PERFECT. Perfect means the mandatory part has been integrally done
    and works without malfunctioning. If you have not passed ALL the
    mandatory requirements, your bonus part will not be evaluated at all.
*/