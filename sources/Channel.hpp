#ifndef CLIENT_H
#define CLIENT_H

#include "Client.hpp"
#include "Server.hpp"
#include <vector>

struct Server;

struct Channel {
	typedef std::vector<Client *>::iterator client_iterator;

	std::string name;
	Client *admin;
	std::vector<Client *> clients;
	std::string key;
	std::string topic;
	size_t limit;
	bool message;
	bool topicMode;
	bool inviteOnly;
	std::vector<Client *> operators;
	Channel();
	Channel(const Channel &src);

	void sendAll(const std::string &message);
	std::vector<std::string> getNicknames();
	Channel(const std::string &name, const std::string &key, Client &admin);
	void handleClientRemoval(Client *client);
	bool isInChannel(Client &channel);
	bool isOperator(Client &client);
	void removeOperator(Client &client);
	void addOperator(Client &client);
	Client *getClientByNick(std::string nickname);
	bool isAdmin(Client &client);
	void kick(Client &client, Client &target, const std::string &reason);
	void removeClient(Client &client);
	void broadcast(const std::string &message, Client *exclude);
	void broadcast(const std::string &message);
	bool channelIsFull(void);
	void replyWho(Client &client, int mode);
	void sending(Client &C, const std::string& msg, const std::string& cmd);
};

#endif