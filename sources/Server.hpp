#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "Parser.hpp"
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Command.hpp"
#include "Utils.hpp"
#include "Socket.hpp"

struct Channel;
struct Client;

struct pollfd_iterator_invalidated : std::exception {};

struct Server {
	const std::string port;
	const std::string pass;
	Socket sock;

	// Event holders.
	std::vector<pollfd> fds;

	std::map<int, Client> clients;
	std::vector<Channel *> channels;

	Server(const std::string &port, const std::string &pass);
	~Server();
	int initializeSocket();
	void start();
	void disconnectClient(int fd);
	void connect_client();
	void handle_client_message(int fd);
	struct std::vector<message> get_client_message(int fd);
	void dispatch(Client &c, message m);
	Client *getClient(const std::string &nickname);
	Channel *getChannel(const std::string &name);
	Channel *addChannel(const std::string &name, const std::string &key,
			    Client &client);
	void updateNicknameInClients(int fd, const std::string& newNickname);
	void updateNicknameInChannels(const std::string& oldNickname, const std::string& newNickname);
};

#endif