#pragma once

#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <vector>
#include <netdb.h>
#include <map>
#include "Client.hpp"
#include <unistd.h>
#include "Channel.hpp"
#include "Parser.hpp"
#define MAX_CLIENTS 100

class Server {
	private:    
        int	running;
        int sock;
		const std::string       port;
		const std::string       host;
		const std::string       pass;
        std::vector<pollfd>     fds;
		std::map<int, Client *> clients;
		std::vector<Client> 	channels;
	public:
		Server(const std::string &port, const std::string &pass);
		~Server();
		int		initialize_socket();
		void	start();
		void	disconnect_client(int fd);
		void	connect_client();
		void    handle_client_message(int fd);
		struct message get_client_message(int fd);
};