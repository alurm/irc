#include "Pass.hpp"

Base2::Base2(Server *s, bool auth) : server(s), is_auth(auth) {}

Base2::~Base2() {}

bool Base2::isAuthenticationRequired() const { return is_auth; }

// PASS
Pass::Pass(Server *server, bool auth) : Base2(server, auth) {}

Pass::~Pass() {}

void Pass::execute(Client *client, std::vector<std::string> args) {
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
Join::Join(Server *server, bool auth) : Base2(server, auth) {}

Join::~Join() {}

void Join::execute(Client *client, std::vector<std::string> args) {
	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "JOIN"));
		return;
	}
	std::string name = args[0];
	std::string pass = args.size() > 1 ? args[1] : "";
	if (name[0] != '#' && name[0] != '&') {
		client->respondWithPrefix(
		    IRCResponse::ERR_BADCHANMASK(client->getNickname(), name));
		return;
	}
	Channel *channel = server->getChannel(name);
	channel = server->getChannel(name);
	if (!channel)
		channel = server->addChannel(name, pass, client);
	if (channel) {
		client->respondWithPrefix(IRCResponse::ERR_USERONCHANNEL(
		    client->getNickname(), name));
		return;
	}
	if (channel->isInviteOnly()) {
		client->respondWithPrefix(IRCResponse::ERR_INVITEONLYCHAN(
		    client->getNickname(), name));
		return;
	}
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
	if (!client->nickIsCorrect(nickname)) {
		client->respondWithPrefix(IRCResponse::ERR_ERRONEUSNICKNAME(
		    client->getNickname(), nickname));
		return;
	}
	if (server->getClient(nickname)) {
		client->respondWithPrefix(
		    IRCResponse::ERR_NICKNAMEINUSE(client->getNickname()));
		return;
	}
	client->setNickname(nickname);
	client->sendWelcomeMessage();
}

// USER
User::User(Server *server, bool auth) : Base2(server, auth) {}

User::~User() {}

void User::execute(Client *client, std::vector<std::string> args) {
	if (client->isInRegisteredState()) {
		client->respondWithPrefix(
		    IRCResponse::ERR_ALREADYREGISTERED(client->getNickname()));
		return;
	}

	if (args.size() < 4) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "USER"));
		return;
	}
	std::cout << "username is " << args[0] << std::endl;
	std::cout << "realname is " << args[3] << std::endl;
	client->setUsername(args[0]);
	client->setRealname(
	    args[3]); // or we should till end consider the realname
	client->sendWelcomeMessage();
}

// USER guest 0 * :John Doe
// USER guest FDGDFGD SGSGSDGF :John Doe

// USER guest FDGDFGD SGSGSDGF John Doe

// QUIT
Quit::Quit(Server *server, bool auth) : Base2(server, auth) {}

Quit::~Quit() {}

void Quit::execute(Client *client, std::vector<std::string> args) {
	std::string reason = args.empty() ? "Leaving..." : args.at(0);
	client->respondWithPrefix(
	    IRCResponse::RPL_QUIT(client->getPrefix(), reason));
	server->disconnectClient(client->getFd());
}

// MODE

Mode::Mode(Server *server, bool auth) : Base2(server, auth) {}

Mode::~Mode() {}

void Mode::execute(Client *client, std::vector<std::string> args) {
	if (args.size() < 2) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "MODE"));
		return;
	}

	std::string target = args.at(0);

	Channel *channel = server->getChannel(target);
	if (!channel) {
		client->respondWithPrefix(IRCResponse::ERR_NOSUCHCHANNEL(
		    client->getNickname(), target));
		return;
	}
	if (!channel->isInChannel(client)) {
		client->respondWithPrefix(IRCResponse::ERR_NOTONCHANNEL(
		    client->getNickname(), target));
		return;
	}
	if (!channel->isOperator(client)) {
		client->respondWithPrefix(IRCResponse::ERR_CHANOPRIVSNEEDED(
		    client->getNickname(), target));
		return;
	}

	// else {
	// 	client->respondWithPrefix(IRCResponse::ERR_UNKNOWNMODE(
	// 	    client->getNickname(), mode,
	// 	    " :is an unknown mode char to me"));
	// 	return;
	// }
}
// topic
Topic::Topic(Server *server, bool auth) : Base2(server, auth) {}

Topic::~Topic() {}

void Topic::execute(Client *client, std::vector<std::string> args) {}