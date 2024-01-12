#include "Pass.hpp"

Base2::Base2(Server *s, bool auth) : server(s), is_auth(auth) {}

Base2::~Base2() {}

bool Base2::isAuthenticationRequired() const { return is_auth; }

// PASS
Pass::Pass(Server *server, bool auth) : Base2(server, auth) {}

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
Join::Join(Server *server, bool auth) : Base2(server, auth) {}

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
	// TODO write logic about the nickname change
	//  if (server->getClient(newNickname, true)) {
	//  	client->setNickname(oldNickname);
	//  	client->respondWithPrefix(
	//  	    IRCResponse::ERR_NICKNAMEINUSE(client->getNickname()));
	//  	return;
	//  }

	// client->setNickname(newNickname);
	// client->respondWithPrefix(
	//     IRCResponse::NICK_CHANGE_CONFIRMATION(oldNickname, newNickname));
	// Channel *channels = getChannel();
	// std::string message = ":" + oldNickname + " NICK :" + newNickname;
	// server->broadcastToChannelsInClientChannels(message, channels);
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
		client->respondWithPrefix(
		    IRCResponse::ERR_NEEDMOREPARAMS(client->getNickname(), "MODE"));
		return;
	}

	std::string target = args.at(0);

	Channel *channel =
	    server->getChannel(target); // MODE on clients not implemented
	if (!channel) {
		client->respondWithPrefix(
		    IRCResponse::ERR_NOSUCHCHANNEL(client->getNickname(), target));
		return;
	}

	if (channel->getAdmin() != client) {
		client->respondWithPrefix(
		    IRCResponse::ERR_CHANOPRIVSNEEDED(client->getNickname(), target));
		return;
	}

	// changing the mode

	int i = 0, p = 2;
	char c;

	while ((c = args[1][i])) {
		char prev_c = i > 0 ? args[1][i - 1] : '\0';
		bool active = prev_c == '+';

		switch (c) {
		case 'n': {
			channel->setMessage(active);
			channel->sendAll(
			    IRCResponse::RPL_MODE(client->getPrefix(), channel->getName(),
				     (active ? "+n" : "-n"), ""));
			break;
		}
		case 'l': {
			channel->setLimit(active ? atoi(args[p].c_str()) : 0);
			channel->sendAll(IRCResponse::RPL_MODE(
			    client->getPrefix(), channel->getName(),
			    (active ? "+l" : "-l"), (active ? args[p] : "")));
			p += active ? 1 : 0;
			break;
		}
		case 'k': {
			channel->setKey(active ? args[p] : "");
			channel->sendAll(IRCResponse::RPL_MODE(
			    client->getPrefix(), channel->getName(),
			    (active ? "+k" : "-k"), (active ? args[p] : "")));
			p += active ? 1 : 0;
			break;
		}
		default:
			break;
		}
		i++;
	}
}