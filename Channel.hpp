#pragma once

#include "Client.hpp"
#include "Server.hpp"

class Client;
class Server;

class Channel {
	typedef std::vector<Client *>::iterator client_iterator;

      private:
	std::string name;
	Client *admin;
	std::vector<Client *> clients;
	std::string key;
	size_t limit;
	bool message;

	Channel();
	Channel(const Channel &src);

      public:
	void sendAll(const std::string &message);
	std::string getName() const;
	void add_client(Client *client) { clients.push_back(client); }
	std::string getKey() const;
	size_t getLimit() const;
	size_t getClientCount() const;
	std::vector<std::string> getNicknames();
	Channel(const std::string &name, const std::string &key, Client *admin);
	~Channel();
	void handleClientRemoval(Client *client);
};
