#include "Command.hpp"

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
	// if (name[0] != '#' && name[0] != '&') {
	// 	client->respondWithPrefix(
	// 	    IRCResponse::ERR_BADCHANMASK(client->getNickname(), name));
	// 	return;
	// }
	Channel *channel = server->getChannel(name);

	if (!channel)
	{
		channel = server->addChannel(name, pass, client);
		
	}
	if (channel->isInChannel(client)) {
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
	std::cout << "befor join handle\n";
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
	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "MODE"));
		return;
	}

	std::string channel_name = args[0];
	Channel *channel = server->getChannel(channel_name);

	if (!channel) {
		client->respondWithPrefix(IRCResponse::ERR_NOSUCHCHANNEL(
		    client->getNickname(), channel_name));
		return;
	}

	if (!channel->isOperator(client)) {
		client->respondWithPrefix(IRCResponse::ERR_CHANOPRIVSNEEDED(
		    client->getNickname(), channel_name));
		return;
	}
	if (args.size() > 1) {
		std::string mode = args[1];

		// MODE +/- i
		if (mode == "i" || mode == "+i" || mode == "-i") {
			bool condition = (mode == "-i") ? false : true;
			mode = (mode == "i") ? "+i" : mode;

			channel->setInviteOnly(condition);
			client->sendWithLineEnding(IRCResponse::RPL_MODE(
			    client->getPrefix(), channel_name, mode));
			client->respondWithPrefix(
			    IRCResponse::RPL_CHANNELMODEIS(
				client->getNickname(), channel_name, mode));
		}

		// MODE +/- t
		else if (mode == "t" || mode == "+t" || mode == "-t") {
			bool condition = (mode == "-t") ? false : true;
			mode = (mode == "t") ? "+t" : mode;

			channel->setTopicMode(condition);
			client->sendWithLineEnding(IRCResponse::RPL_MODE(
			    client->getPrefix(), channel_name, mode));
			client->respondWithPrefix(
			    IRCResponse::RPL_CHANNELMODEIS(
				client->getNickname(), channel_name, mode));
		}

		// MODE +/- k
		else if (mode == "k" || mode == "+k" || mode == "-k") {
			std::string key;
			if (args.size() > 2)
				key = args[2];

			if (mode != "-k") {
				if (channel->getKey() != "") {
					client->respondWithPrefix(
					    IRCResponse::ERR_KEYSET(
						client->getNickname(), mode));
					return;
				}

				channel->setKey(key);
				client->sendWithLineEnding(
				    IRCResponse::RPL_MODE(client->getPrefix(),
							  channel_name,
							  "+k " + key));
			} else {
				if (channel->getKey() == key) {
					channel->setKey("");
					client->sendWithLineEnding(
					    IRCResponse::RPL_MODE(
						client->getPrefix(),
						channel_name, "-k "));
				} else {
					client->respondWithPrefix(
					    IRCResponse::ERR_BADCHANNELKEY(
						client->getNickname(),
						mode + " :invalid key"));
					return;
				}
			}
			client->respondWithPrefix(
			    IRCResponse::RPL_CHANNELMODEIS(
				client->getNickname(), channel_name, mode));
		}
		// MODE +/- o
		else if (mode == "o" || mode == "+o" || mode == "-o") {
			if (args.size() < 3) {
				client->respondWithPrefix(
				    IRCResponse::ERR_NEEDMOREPARAMS(
					client->getNickname(), "MODE"));
				return;
			}

			std::string nickname = args[2];
			Client *cli = channel->getClientByNick(nickname);
			if (!cli) {
				client->respondWithPrefix(
				    IRCResponse::ERR_USERNOTINCHANNEL(
					client->getNickname(), nickname,
					channel_name));
				return;
			}

			if (mode != "-o") {
				cli->sendWithLineEnding(IRCResponse::RPL_MSG(
				    client->getPrefix(), "MODE", channel_name,
				    ":you are now a channel operator"));
				channel->addOperator(cli);
			} else {
				if (!channel->isAdmin(cli) &&
				    channel->isOperator(cli)) {
					cli->sendWithLineEnding(
					    IRCResponse::RPL_MSG(
						client->getPrefix(), "MODE",
						channel_name,
						":you are no longer a channel "
						"operator"));
					channel->removeOperator(cli);
				}
			}
			client->respondWithPrefix(
			    IRCResponse::RPL_CHANNELMODEIS(
				client->getNickname(), channel_name, mode));
		}
		// MODE +/- l
		else if (mode == "l" || mode == "+l" || mode == "-l") {
			if (mode != "-l") {
				if (args.size() < 3) {
					client->respondWithPrefix(
					    IRCResponse::ERR_NEEDMOREPARAMS(
						client->getNickname(), "MODE"));
					return;
				}

				int new_limit = std::atoi(args[2].c_str());
				if (new_limit < 1) {
					client->respondWithPrefix(
					    IRCResponse::ERR_UNKNOWNMODE(
						client->getNickname(), mode,
						" :limit must be greater than "
						"0"));
					return;
				}
				channel->setChannelLimit(new_limit);
			} else {
				channel->setChannelLimit(
				    0); // unlimit in my case defaul is 10
					// ?????????
			}

			client->respondWithPrefix(
			    IRCResponse::RPL_CHANNELMODEIS(client->getPrefix(),
							   channel_name, mode));
		} else if (mode == "b") {
			return;
		} else {
			client->respondWithPrefix(IRCResponse::ERR_UNKNOWNMODE(
			    client->getNickname(), mode,
			    " :is unknown mode char to me"));
			return;
		}
	}
}

// topic
Topic::Topic(Server *server, bool auth) : Base2(server, auth) {}

Topic::~Topic() {}

void Topic::execute(Client *client, std::vector<std::string> args) {
	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "TOPIC"));
		return;
	}
	std::string channel_name = args[0];
	Channel *channel = server->getChannel(channel_name);
	if (!channel) {
		client->respondWithPrefix(IRCResponse::ERR_NOSUCHCHANNEL(
		    client->getNickname(), channel_name));
		return;
	}
	if (!channel->isInChannel(client)) {
		client->respondWithPrefix(IRCResponse::ERR_NOTONCHANNEL(
		    client->getNickname(), channel_name));
		return;
	}
	if (!channel->isOperator(client)) {
		client->respondWithPrefix(IRCResponse::ERR_CHANOPRIVSNEEDED(
		    client->getNickname(), channel_name));
		return;
	}
	if (channel->topicModeIsOn() == false) {
		client->sendWithLineEnding(
		    IRCResponse::ERR_NOCHANMODES(channel_name));
		return;
	}
	if (args.size() == 1) {
		std::string topic = channel->getTopic();
		if (topic.empty()) {
			client->sendWithLineEnding(
			    IRCResponse::RPL_NOTOPIC(channel_name));
		} else {
			client->sendWithLineEnding(
			    IRCResponse::RPL_TOPIC(channel_name, topic));
		}
	} else {
		std::string topic = args[1];
		channel->setTopic(topic);
	}
}

// Ping

Ping::Ping(Server *server, bool auth) : Base2(server, auth) {}

Ping::~Ping() {}

void Ping::execute(Client *client, std::vector<std::string> args) {
	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "PONG"));
		return;
	}
	client->sendWithLineEnding(
	    IRCResponse::RPL_PING(client->getPrefix(), args[0]));
}

// Cap >?>

// idk for what is it

// PrivMsg

PrivMsg::PrivMsg(Server *server, bool auth) : Base2(server, auth) {}

PrivMsg::~PrivMsg() {}

void PrivMsg::execute(Client *client, std::vector<std::string> args) {
	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "PRIVMSG"));
		return;
	}
	if (args.size() < 2) {
		client->respondWithPrefix(
		    IRCResponse::ERR_NOTEXTTOSEND(client->getNickname()));
		return;
	}
	std::vector<std::string> targets;
	std::string keys = args[0];

	keys += ',';
	size_t start = 0;
	size_t index = keys.find(',', start);

	while (index != std::string::npos) {
		targets.push_back(keys.substr(start, index - start));
		start = index + 1;
		index = keys.find(',', start);
	}
	size_t i = 1;
	std::string message = args[i++];
	for (; i < args.size(); ++i)
		message.append(" " + args[i]);
	for (size_t i = 0; i < targets.size(); ++i) {
		if (targets[i][0] == '#' || targets[i][0] == '&') {
			Channel *channel = server->getChannel(targets[i]);
			if (!channel) {
				client->respondWithPrefix(
				    IRCResponse::ERR_NOSUCHNICK(
					client->getNickname(), targets[i]));
				return;
			}
			if (!channel->isInChannel(client)) {
				client->respondWithPrefix(
				    IRCResponse::ERR_CANNOTSENDTOCHAN(
					client->getNickname(), targets[i]));
				return;
			}
			channel->sending(client, message, "PRIVMSG");
		} else {
			Client *cli = server->getClient(targets[i]);
			if (!cli) {
				client->respondWithPrefix(
				    IRCResponse::ERR_NOSUCHNICK(
					client->getNickname(), targets[i]));
				return;
			}

			client->sendWithLineEnding(
			    IRCResponse::RPL_MSG(client->getPrefix(), "PRIVMSG",
						 targets[i], message));
		}
	}
}

// Pong

Pong::Pong(Server *server, bool auth) : Base2(server, auth) {}

Pong::~Pong() {}

void Pong::execute(Client *client, std::vector<std::string> args) {
	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "PONG"));
		return;
	}
	client->sendWithLineEnding(
	    IRCResponse::RPL_PING(client->getPrefix(), args[0]));
}

// Kick

Kick::Kick(Server *server, bool auth) : Base2(server, auth) {}

Kick::~Kick() {}

void Kick::execute(Client *client, std::vector<std::string> args) {
	if (args.size() < 2) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "KICK"));
		return;
	}

	std::string name = args[0];
	std::string target = args[1];
	std::string reason = "No reason specified!";

	if (args.size() >= 3 && (args[2][0] != ':' || args[2].size() > 1)) {
		reason = "";

		std::vector<std::string>::iterator it = args.begin();
		std::vector<std::string>::iterator end = args.end();

		while (it != end) {
			reason.append(*it + " ");
			it++;
		}
	}
	Channel *channel = client->getChannel();
	if (!channel || channel->getName() != name) {
		client->respondWithPrefix(
		    IRCResponse::ERR_NOTONCHANNEL(client->getNickname(), name));
		return;
	}

	if (channel->getAdmin() != client) {
		client->respondWithPrefix(IRCResponse::ERR_CHANOPRIVSNEEDED(
		    client->getNickname(), name));
		return;
	}

	Client *dest = server->getClient(target);
	if (!dest) {
		client->respondWithPrefix(
		    IRCResponse::ERR_NOSUCHNICK(client->getNickname(), target));
		return;
	}

	if (!dest->getChannel() || dest->getChannel() != channel) {
		client->respondWithPrefix(IRCResponse::ERR_USERNOTINCHANNEL(
		    client->getNickname(), dest->getNickname(), name));
		return;
	}
	channel->kick(client, dest, reason);
}

// Invite

Invite::Invite(Server *server, bool auth) : Base2(server, auth) {}

Invite::~Invite() {}

void Invite::execute(Client *client, std::vector<std::string> args) {
	if (args.size() < 2) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "WHO"));
		return;
	}

	std::string nickName = args[0];
	std::string channelName = args[1];

	Client *cli = server->getClient(nickName);
	if (!cli) {
		client->respondWithPrefix(IRCResponse::ERR_NOSUCHNICK(
		    client->getNickname(), nickName));
		return;
	}

	Channel *channel = server->getChannel(channelName);
	if (!channel) {
		client->respondWithPrefix(IRCResponse::ERR_NOSUCHCHANNEL(
		    client->getNickname(), channelName));
		return;
	}

	if (!channel->isInChannel(client)) {
		client->respondWithPrefix(IRCResponse::ERR_NOTONCHANNEL(
		    client->getNickname(), channelName));
		return;
	}

	if (!channel->isOperator(client)) {
		client->respondWithPrefix(IRCResponse::ERR_CHANOPRIVSNEEDED(
		    client->getNickname(), channelName));
		return;
	}

	if (channel->isInChannel(cli)) {
		client->respondWithPrefix(IRCResponse::ERR_USERONCHANNEL(
		    client->getNickname(), channelName));
		return;
	}

	if (channel->channelIsFull()) {
		client->respondWithPrefix(IRCResponse::ERR_CHANNELISFULL(
		    client->getNickname(), channelName));
		return;
	}

	cli->sendWithLineEnding(IRCResponse::RPL_INVITE(client->getPrefix(),
							nickName, channelName));
	client->respondWithPrefix(IRCResponse::RPL_INVITING(
	    client->getNickname(), nickName, channelName));
	client->handleChannelJoin(channel);
}

// Part

Part::Part(Server *server, bool auth) : Base2(server, auth) {}

Part::~Part() {}

void Part::execute(Client *client, std::vector<std::string> args) {
	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "PART"));
		return;
	}

	std::string name = args[0];
	Channel *channel = server->getChannel(name);

	if (!channel || !client->getChannel() ||
	    client->getChannel()->getName() != name) {
		client->respondWithPrefix(IRCResponse::ERR_NOSUCHCHANNEL(
		    client->getNickname(), name));
		return;
	}

	client->handleChannelLeave();
}

// who
Who::Who(Server *server, bool auth) : Base2(server, auth) {}

Who::~Who() {}

void Who::execute(Client *client, std::vector<std::string> args) {
	if (args.empty()) {
		client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(
		    client->getNickname(), "WHO"));
		return;
	}

	if (args[0][0] == '#' || args[0][0] == '&') {
		Channel *channel = server->getChannel(args[0]);
		if (!channel) {
			client->respondWithPrefix(
			    IRCResponse::ERR_NOSUCHCHANNEL(
				client->getNickname(), args[0]));
			return;
		}
		if (!channel->isInChannel(client)) {
			client->respondWithPrefix(IRCResponse::ERR_NOTONCHANNEL(
			    client->getNickname(), args[0]));
			return;
		}
		int mode = (args.size() > 1 && args[1] == "o") ? 1 : 0;
		channel->replyWho(client, mode);
		return;
	}

	Client *cli = server->getClient(args[0]);
	if (!cli) {
		client->respondWithPrefix(IRCResponse::ERR_NOSUCHNICK(client->getNickname(), args[0]));
		return;
	}

	client->sendWithLineEnding(
	    IRCResponse::RPL_WHOREPLY(client->getNickname(), "*", client->getUsername(),
				      client->getHostname(), client->getNickname(), "H", client->getRealname()));
	client->sendWithLineEnding(
	    IRCResponse::RPL_ENDOFWHO(client->getNickname(), args[0]));
}
