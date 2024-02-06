#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &key, Client &admin)
    : name(name), admin(&admin), key(key), limit(10), message(false) {
	topic = "";
	inviteOnly = false;
	topicMode = false;
	admin.sendWithLineEnding(IRCResponse::RPL_MSG(admin.getPrefix(), "", name + static_cast<char>(1), "you are the new admin"));
	operators.push_back(&admin);
}

std::vector<std::string> Channel::getNicknames() {
	std::vector<std::string> nicknames;

	for (client_iterator it = clients.begin(); it != clients.end(); ++it) {
		Client *client = *it;

		std::string nick =
		    (client == admin ? "@" : "") + client->nick_name;
		nicknames.push_back(nick);
	}

	return nicknames;
}

void Channel::sendAll(const std::string &message) {
	for (client_iterator it = clients.begin(); it != clients.end(); ++it) {
		(*it)->sendWithLineEnding(message);
	}
}

void Channel::handleClientRemoval(Client *client) {
	client_iterator it = clients.begin();

	while (it != clients.end()) {
		if (*it == client) {
			it = clients.erase(it);
		} else {
			++it;
		}
	}
if (client) {
        client->chan = 0;
	if (client == admin) {
		client->chan = 0;
		if (!clients.empty()) {
			admin = clients[0];
			operators.push_back(clients[0]);
			admin->sendWithLineEnding(IRCResponse::RPL_MSG(admin->getPrefix(), "", name, "you are the new admin"));
			std::string message =
			    client->nick_name +
			    " is now the admin of the channel " + name;
			sending(*admin, message, "PRIVMSG");
			std::cout << message << std::endl;
		} else {
			admin = NULL;
		}
	}
}
}

bool Channel::isInChannel(Client &client) {
	if (std::find(clients.begin(), clients.end(), &client) == clients.end())
		return false;
	return true;
}

bool Channel::isOperator(Client &client) {
	if (std::find(operators.begin(), operators.end(), &client) ==
	    operators.end())
		return false;
	return true;
}

void Channel::addOperator(Client &client) {
	if (isInChannel(client) && !isOperator(client))
		operators.push_back(&client);
}
void Channel::removeOperator(Client &client) {
	if (isOperator(client)) {
		std::vector<Client *>::iterator it =
		    std::find(operators.begin(), operators.end(), &client);
		operators.erase(it);
	}
}
bool Channel::isAdmin(Client &client) {
	if (!isInChannel(client))
		return false;
	return &client == admin;
}

void Channel::kick(Client &client, Client &target, const std::string &reason) {
	this->broadcast(
	    IRCResponse::RPL_KICK(client.getPrefix(), name, target.nick_name, reason));
	this->removeClient(target);
	std::string message = client.nick_name + " kicked " +
			      target.nick_name + " from channel " + name;
	std::cout << message << std::endl;
}

void Channel::removeClient(Client &client) {
	client_iterator it_b = clients.begin();
	client_iterator it_e = clients.end();

	while (it_b != it_e) {
		if (*it_b == &client) clients.erase(it_b);

		it_b++;
	}

	client.chan = 0;

	if (&client == admin) {
		admin = *(clients.begin());

		std::string message = client.nick_name +
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

void Channel::replyWho(Client &client, int mode)
{
    for(size_t i = 0; i < clients.size(); ++i)
    {
        std::string reply = IRCResponse::RPL_WHOREPLY(
        	client.nick_name, name,
        	clients[i]->user_name, clients[i]->host_name,
        	clients[i]->nick_name, "H", clients[i]->real_name
        );

        if (!mode || this->isOperator(client))
            client.sendWithLineEnding(reply);
    }
    client.sendWithLineEnding(IRCResponse::RPL_ENDOFWHO(client.nick_name, name));
}

void Channel::sending(Client &c, const std::string &msg, const std::string &cmd)
{
    for (size_t i = 0; i < clients.size(); ++i)
        if (clients[i] != &c)
            clients[i]->sendWithLineEnding(IRCResponse::RPL_MSG(c.getPrefix(), cmd, name, msg));
}