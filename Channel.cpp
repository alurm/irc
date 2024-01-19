#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &key, Client *admin)
    : name(name), admin(admin), key(key), limit(10), message(false) {
		operators.push_back(admin);
	}

std::string Channel::getName() const { return name; }

std::vector<std::string> Channel::getNicknames() {
	std::vector<std::string> nicknames;

	for (client_iterator it = clients.begin(); it != clients.end(); ++it) {
		Client *client = *it;

		std::string nick =
		    (client == admin ? "@" : "") + client->getNickname();
		nicknames.push_back(nick);
	}

	return nicknames;
}

void Channel::sendAll(const std::string &message) {
	for (client_iterator it = clients.begin(); it != clients.end(); ++it) {
		(*it)->sendWithLineEnding(message);
	}
}

std::string Channel::getKey() const { return key; }

size_t Channel::getLimit() const { return limit; }

size_t Channel::getClientCount() const { return clients.size(); }

void Channel::handleClientRemoval(Client *client) {
	client_iterator it = clients.begin();

	while (it != clients.end()) {
		if (*it == client) {
			it = clients.erase(it);
		} else {
			++it;
		}
	}

	client->setChannel(NULL);

	if (client == admin) {
		if (!clients.empty()) {
			admin = *(clients.begin());

			std::string message =
			    client->getNickname() +
			    " is now the admin of the channel " + name;
			std::cout << message << std::endl;
		} else {
			admin = NULL;
		}
	}
}

void Channel::setMessage(bool flag) { message = flag; }
bool Channel::getMerssage() const { return message; }
void Channel::setLimit(size_t l) { limit = l; }
void Channel::setKey(std::string k) { key = k; }
Client *Channel::getAdmin() const { return admin; }

Channel::~Channel() {}

bool Channel::isInChannel(Client *client) {
	if (std::find(clients.begin(), clients.end(), client) == clients.end())
		return false;
	return true;
}

bool Channel::topicModeIsOn(void) { return topicMode; }

bool Channel::isInviteOnly(void) { return inviteOnly; }
void Channel::setInviteOnly(bool mode) { inviteOnly = mode; }
void Channel::setTopicMode(bool mode) { topicMode = mode; }
bool Channel::isOperator(Client *client) {
	if (std::find(operators.begin(), operators.end(), client) ==
	    operators.end())
		return false;
	return true;
}

void Channel::addOperator(Client *client) {
	if (isInChannel(client) && !isOperator(client))
		operators.push_back(client);
}
void Channel::removeOperator(Client *client) {
	if (isOperator(client)) {
		std::vector<Client *>::iterator it =
		    std::find(operators.begin(), operators.end(), client);
		operators.erase(it);
	}
}
bool Channel::isAdmin(Client *client) {
	if (!isInChannel(client))
		return false;
	return client == admin;
}
void Channel::setChannelLimit(int l) { limit = l; }

std::string Channel::getTopic(void) { return topic; }
void Channel::setTopic(const std::string &t) { topic = t; }

void Channel::sending(Client *client, const std::string &msg,
		      const std::string &cmd) {
	for (size_t i = 0; i < clients.size(); ++i)
		if (clients[i] != client)
			clients[i]->sendWithLineEnding(IRCResponse::RPL_MSG(
			    client->getPrefix(), cmd, name, msg));
}

void Channel::kick(Client *client, Client *target, const std::string &reason) {
	this->broadcast(
	    IRCResponse::RPL_KICK(client->getPrefix(), name, target->getNickname(), reason));
	this->removeClient(target);
	std::string message = client->getNickname() + " kicked " +
			      target->getNickname() + " from channel " + name;
	std::cout << message << std::endl;
}

void Channel::removeClient(Client *client) {
	client_iterator it_b = clients.begin();
	client_iterator it_e = clients.end();

	while (it_b != it_e) {
		if (*it_b == client)
			clients.erase(it_b);

		it_b++;
	}
	client->setChannel(NULL);
	if (client == admin) {
		admin = *(clients.begin());

		std::string message = client->getNickname() +
				      " is now the admin of the channel " +
				      name;
		std::cout << message << std::endl;
	}
}

void   Channel::broadcast(const std::string& message, Client* exclude)
{
    client_iterator it_b = clients.begin();
    client_iterator it_e = clients.end();

    while (it_b != it_e)
    {
        if (*it_b == exclude)
        {
            it_b++;
            continue;
        }

        (*it_b)->sendWithLineEnding(message);
        it_b++;
    }
}

void     Channel::broadcast(const std::string& message)
{
    client_iterator it_b = clients.begin();
    client_iterator it_e = clients.end();

    while (it_b != it_e)
    {
        (*it_b)->sendWithLineEnding(message);
        it_b++;
    }
}

bool Channel::channelIsFull(void)
{
    if (limit == 0)
        return false;

    return limit <= clients.size();
}

void Channel::replyWho(Client *client, int mode)
{
    std::string replay;
    std::vector<std::string> atribut;
    for(size_t i = 0; i < clients.size(); ++i)
    {
        replay = IRCResponse::RPL_WHOREPLY(client->getNickname(), name, clients[i]->getUsername(), clients[i]->getHostname(), clients[i]->getNickname(), "H", clients[i]->getRealname());

        if (!mode || this->isOperator(client))
            client->sendWithLineEnding(replay);
    }
    client->sendWithLineEnding(IRCResponse::RPL_ENDOFWHO(client->getNickname(), name));
}
void Channel::add_client(Client *client) { clients.push_back(client); }