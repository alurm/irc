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

	public:
		Server(const std::string &port, const std::string &pass);
		~Server();
		int		initialize_socket();
		void	start();
		void	disconnect_client();
		void	connect_client();
};