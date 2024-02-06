#include "Client.hpp"

Client::Client(int fd, int port, const std::string &hostname)
    : fd(fd), port(port), input_buffer_offset(0), output_buffer_offset(0), state(reading), host_name(hostname), chan(NULL),
      status(client_state::HANDSHAKE) {}

Client::~Client() {}

bool Client::nickIsCorrect(std::string buffer) {
	std::string notAllowed = " ,*?!@$:#.";
	std::size_t pos = buffer.find_first_of(notAllowed);
	if (pos != std::string::npos) {
		return false;
	}
	return true;
}

std::string Client::getPrefix() const {
	std::string username = user_name.empty() ? "" : "!" + user_name;
	std::string hostname = host_name.empty() ? "" : "@" + host_name;
	return nick_name + username + hostname;
}

void Client::sendWithLineEnding(const std::string &message) {
	std::string buffer = message + "\r\n";
	// const char *data = buffer.c_str();
	// int length = buffer.length();

	output_buffer.append(buffer);
	state = writing;
}

void Client::respondWithPrefix(const std::string &message) {
	this->sendWithLineEnding(":" + getPrefix() + " " + message);
}

void Client::sendWelcomeMessage() {
	if (status != client_state::LOGIN || user_name.empty() ||
	    real_name.empty() || nick_name.empty()) {
		return;
	}

	status = client_state::REGISTERED;
	this->respondWithPrefix(IRCResponse::RPL_WELCOME(nick_name));

	std::ostringstream welcomeMessage;
	welcomeMessage << host_name << ":" << port << " is now known as "
		       << nick_name << ".";
	std::cout << welcomeMessage.str() << std::endl;
}

void Client::handleChannelJoin(Channel *channel) {
	if (!channel || channel->isInChannel(*this)) {
		return;
	}
	channel->clients.push_back(this);
	chan = channel;

	std::string users;
	std::vector<std::string> nicknames = chan->getNicknames();
	for (std::vector<std::string>::iterator it = nicknames.begin();
	     it != nicknames.end(); ++it) {
		users += *it + " ";
	}
	std::string joinedUsers = users.empty() ? "" : " " + users;

	for (size_t i = 0; i < channel->clients.size(); ++i)
		channel->clients[i]->sendWithLineEnding(
		    IRCResponse::RPL_JOIN(this->getPrefix(), channel->name));

	std::string topic = channel->topic;
	// if (topic.empty()) {
	// 	this->sendWithLineEnding(IRCResponse::RPL_MSG(
	// 	    this->getPrefix(), "PRIVMSG", channel->name,
	// 	    IRCResponse::RPL_NOTOPIC(channel->name)));
	// } else {
	// 	this->sendWithLineEnding(
	// 	    IRCResponse::RPL_TOPIC(channel->name, topic));
	// }
    if (topic.empty())            
        this->sendWithLineEnding(IRCResponse::RPL_NOTOPIC(channel->name + static_cast<char>(1)));
    else            
        this->sendWithLineEnding(IRCResponse::RPL_TOPIC(channel->name + static_cast<char>(1), topic));

	// this->sendWithLineEnding(IRCResponse::RPL_MSG(
	//     this->getPrefix(), "PRIVMSG", channel->name,
	//     IRCResponse::RPL_NAMREPLY(nick_name, channel->name, users)));
	// this->sendWithLineEnding(IRCResponse::RPL_MSG(
	//     this->getPrefix(), "PRIVMSG", channel->name,
	//     IRCResponse::RPL_ENDOFNAMES(nick_name, channel->name)));
	this->sendWithLineEnding(IRCResponse::RPL_NAMREPLY(nick_name, channel->name + static_cast<char>(1), users));
    this->sendWithLineEnding(IRCResponse::RPL_ENDOFNAMES(nick_name, channel->name + static_cast<char>(1)));
	std::string message = nick_name + " has joined the channel " +
			      channel->name + joinedUsers;
	std::cout << message << std::endl;
}

void Client::handleChannelLeave() {
	if (!chan)
		return;
	std::string prefix = getPrefix();
	std::string nick = nick_name;
	std::string name = chan->name;
	chan->handleClientRemoval(this);
	if(chan) {
		chan->sendAll(IRCResponse::RPL_PART(prefix, chan->name));
	}

	std::string message = nick + " has left the channel " + name;
	std::cout << message << std::endl;

}

Client *Channel::getClientByNick(std::string nickname) {
	for (std::vector<Client *>::iterator it = clients.begin();
	     it != clients.end(); ++it)
		if ((*it)->nick_name == nickname)
			return *it;

	return NULL;
}

optional<message> Client::optional_message() {
	if (input_buffer == "") {
		char buffer[1024];
		ssize_t bytesRead = recv(fd, buffer, 1024, 0);
		if (bytesRead < 0)
		throw std::runtime_error(
			"Error while reading buffer from a client!");

		input_buffer.append(buffer, bytesRead);
	}

	while (input_buffer_offset != input_buffer.size()) {
		std::vector<lexeme> lexemes = lex(input_buffer[input_buffer_offset], lexer);
		input_buffer_offset++;
		for (std::vector<lexeme>::iterator it = lexemes.begin(); it != lexemes.end(); it++) {
			optional<message> optional_message = parse(*it, parser);
			if (optional_message.has_value) {
				return optional_message;
			}
		}
	}

	input_buffer = std::string();
	input_buffer_offset = 0;
	return optional<message>();
}
