#ifndef CLIENT_H
#define CLIENT_H

#include "Client.hpp"
#include "Server.hpp"
#include <vector>
class Server;
class Client;

class Channel {
	typedef std::vector<Client *>::iterator client_iterator;
      private:
	std::string name;
	Client *admin;
	std::vector<Client *> clients;
	std::string key;
	std::string  topic;
	size_t limit;
	bool message;
	bool topicMode;
	bool inviteOnly;
	std::vector<Client*> operators;
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
	void setMessage(bool flag);
	void setLimit(size_t l);
	void setKey(std::string k);
	bool getMerssage() const;
	bool topicModeIsOn(void);
	Client *getAdmin() const;
	bool isInChannel(Client* channel);
	bool isInviteOnly(void);
	void setInviteOnly(bool mode);
	void setTopicMode(bool mode);
	bool isOperator(Client *client);
	void removeOperator(Client *client);
	void addOperator(Client *client);

};

#endif