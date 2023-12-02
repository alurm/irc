#include "IRCServer.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password> " << std::endl;
        return EXIT_FAILURE;
    }

    int port = std::atoi(argv[1]);
    std::string password = argv[2];

    IRCServer ircServer(port, password);
    // ircServer.acceptConnections();

    return 0;
}