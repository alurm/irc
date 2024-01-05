#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <unistd.h>
#include "Channel.hpp"
#include "Client.hpp"
#include "Parser.hpp"
#define MAX_CLIENTS 100

class Client; 
class Channel;

class Server {
	private:    
        int	running;
        int sock;
		const std::string       port;
		const std::string       host;
		const std::string       pass;
        std::vector<pollfd>     fds;
		std::map<int, Client *> clients;
		std::vector<Channel *> 	channels;
	public:
		Server(const std::string &port, const std::string &pass);
		~Server();
		int		initializeSocket();
		void	start();
		void	disconnectClient(int fd);
		void	connect_client();
		void    handle_client_message(int fd);
		struct message get_client_message(int fd);


		std::string     getPassword() const;
        Client*         getClient(const std::string &nickname);
        Channel*        getChannel(const std::string &name);



		Channel*        addChannel(const std::string &name, const std::string &key, Client *client);

};

#endif