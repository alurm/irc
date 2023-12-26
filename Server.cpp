#include "Server.hpp"
#include "Client.hpp"

Server::Server(const std::string &port, const std::string &pass) 
    : host("127.0.0.1"), port(port), pass(pass)
{
    running = 1;
    sock = initialize_socket();
}

int Server::initialize_socket() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        throw std::runtime_error("Error: Unable to open the socket.");
    }

    int optval = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) {
        throw std::runtime_error("Error: Unable to set socket options.");
    }

    if (fcntl(sock_fd, F_SETFL, O_NONBLOCK)) {
        throw std::runtime_error("Error: Unable to set socket as non-blocking.");
    }

    struct sockaddr_in serv_addr = {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(port.c_str()));

    if (bind(sock_fd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0) {
        throw std::runtime_error("Error: Failed to bind the socket.");
    }

    if (listen(sock_fd, MAX_CLIENTS) < 0) {
        throw std::runtime_error("Error: Failed to start listening on the socket.");
    }

    return sock_fd;
}


Server::~Server() 
{
    // for (size_t i = 0; i < _channels.size(); i++)
    //     delete _clients[i];
}

void Server::connect_client() {
    sockaddr_in addr = {};
    socklen_t size = sizeof(addr);

    int fd = accept(sock, reinterpret_cast<sockaddr*>(&addr), &size);
    if (fd < 0) {
        throw std::runtime_error("Error while accepting a new client!");
    }

    pollfd pfd = { fd, POLLIN, 0 };
    fds.push_back(pfd);

    char hostname[NI_MAXHOST];
    if (getnameinfo(reinterpret_cast<sockaddr*>(&addr), sizeof(addr),
                    hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0) {
        throw std::runtime_error("Error while getting a hostname on a new client!");
    }

    Client* client = new Client(fd, ntohs(addr.sin_port), std::string(hostname));
    clients.insert(std::make_pair(fd, client));

    char message[1000];
    sprintf(message, "%s:%d has connected.", client->get_hostname().c_str(), client->get_port());
    std::cout << message;
}


void	Server::disconnect_client()
{
}

void Server::start() {
    pollfd srv = {sock, POLLIN, 0};
    fds.push_back(srv);

    std::cout << "Server is running...\n";
    std::vector<pollfd>::iterator it;

    while (running) {
        if (poll(&fds[0], fds.size(), -1) < 0) {
            throw std::runtime_error("Error while polling from fd!");
        }

        for (it = fds.begin(); it != fds.end(); ++it) {
            if (it->revents == 0) {
                continue; 
            }

            if (it->revents & POLLHUP) {
				std::cout << "in disconnect\n";
				//handle disconnect
                disconnect_client();
                break;
            }

            if (it->revents & POLLIN) {
                if (it->fd == sock) {
					std::cout << "in connect\n";
                    connect_client();
                    break;
                }

                // on_client_message(it->fd);
            }
        }
    }
}
