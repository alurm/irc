#include "Pass.hpp"

Base2::Base2(Server *s, bool auth) : server(s), is_auth(auth) {}

Base2::~Base2() {}

bool Base2::isAuthenticationRequired() const { return is_auth; }

// PASS
Pass::Pass(Server *srv, bool auth) : Base2(srv, auth) {}

Pass::~Pass() {}

void Pass::execute(Client *client, std::vector<std::string> args) {
	std::cout << "in execute of pass\n";
	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "PASS"));
		return;
	}

	if (client->isInRegisteredState()) {
		client->respondWithPrefix(
		    IRCResponse::ERR_ALREADYREGISTERED(client->getNickname()));
		return;
	}

	if (server->getPassword() != args[0]) {
		client->respondWithPrefix(
		    IRCResponse::ERR_PASSWDMISMATCH(client->getNickname()));
		return;
	}

	client->setState(client_state::LOGIN);
}

// JOIN
Join::Join(Server *srv, bool auth) : Base2(srv, auth) {}

Join::~Join() {}

void Join::execute(Client *client, std::vector<std::string> args) {
	std::cout << "In join\n";

	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "JOIN"));
		return;
	}
	std::cout << "not empty\n";
	std::string name = args[0];
	std::string pass = args.size() > 1 ? args[1] : "";
	std::cout << "Before channel get\n";
	Channel *channel = client->getChannel();
	std::cout << "After channel get\n";
	if (channel) {
		client->respondWithPrefix(IRCResponse::ERR_TOOMANYCHANNELS(
		    client->getNickname(), name));
		return;
	}

	channel = server->getChannel(name);
	if (!channel)
		channel = server->addChannel(name, pass, client);

	if (channel->getLimit() > 0 &&
	    channel->getClientCount() >= channel->getLimit()) {
		client->respondWithPrefix(IRCResponse::ERR_CHANNELISFULL(
		    client->getNickname(), name));
		return;
	}

	if (channel->getKey() != pass) {
		client->respondWithPrefix(IRCResponse::ERR_BADCHANNELKEY(
		    client->getNickname(), name));
		return;
	}

	client->handleChannelJoin(channel);
}

// NICK

Nick::Nick(Server *server, bool auth) : Base2(server, auth) {}

Nick::~Nick() {}

void Nick::execute(Client *client, std::vector<std::string> args) {
	if (args.empty() || args[0].empty()) {
		client->respondWithPrefix(
		    IRCResponse::ERR_NONICKNAMEGIVEN(client->getNickname()));
		return;
	}

	std::string nickname = args[0];

	if (server->getClient(nickname)) {
		client->respondWithPrefix(
		    IRCResponse::ERR_NICKNAMEINUSE(client->getNickname()));
		return;
	}

	// std::string oldNickname = client->getNickname();

	client->setNickname(nickname);
	//TODO write logic about the nickname change
	// if (server->getClient(newNickname, true)) {
	// 	client->setNickname(oldNickname);
	// 	client->respondWithPrefix(
	// 	    IRCResponse::ERR_NICKNAMEINUSE(client->getNickname()));
	// 	return;
	// }

	// client->setNickname(newNickname);
	// client->respondWithPrefix(
	//     IRCResponse::NICK_CHANGE_CONFIRMATION(oldNickname, newNickname));
    // Channel *channels = getChannel();
    // std::string message = ":" + oldNickname + " NICK :" + newNickname;
	// server->broadcastToChannelsInClientChannels(message, channels);
	client->sendWelcomeMessage();
}


// USER
User::User(Server* srv, bool auth) : Base2(srv, auth) {}

User::~User() {}


void    User::execute(Client* client, std::vector<std::string> args)
{
    if (client->isInRegisteredState())
    {
        client->respondWithPrefix(IRCResponse::ERR_ALREADYREGISTERED(client->getNickname()));
        return;
    }

    if (args.size() < 4) 
    {
        client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(client->getNickname(), "USER"));
        return;
    }

    client->setUsername(args[0]);
    client->setRealname(args[3]);
    client->sendWelcomeMessage();
}
