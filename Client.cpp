#include "Client.hpp"

Client::Client(int fd, int port, const std::string &hostname)
    : fd(fd), port(port), host_name(hostname), chan(NULL), status(client_state::HANDSHAKE){}

Client::~Client() {}

int Client::getPort() const { return port; }
int Client::getFd() const { 

    return fd; 
}
std::string Client::getNickname() const { return nick_name; }
std::string Client::getUsername() const { return user_name; }
std::string Client::getRealname() const { return real_name; }
std::string Client::getHostname() const { return host_name; }
std::string Client::getPrefix() const {
	std::string username = user_name.empty() ? "" : "!" + user_name;
	std::string hostname = host_name.empty() ? "" : "@" + host_name;
	return nick_name + username + hostname;
}
Channel *Client::getChannel() const { return chan; }

void Client::setNickname(const std::string &nickname) { nick_name = nickname; }
void Client::setUsername(const std::string &username) { user_name = username; }
void Client::setRealname(const std::string &realname) { real_name = realname; }
void Client::setState(client_state::ClientState state) { status = state; }
void Client::setChannel(Channel *channel) { chan = channel; }
bool Client::isInRegisteredState() const { return status == client_state::REGISTERED; }


void Client::sendWithLineEnding(const std::string &message) const {
    std::string buffer = message + "\r\n";
    const char *data = buffer.c_str();
    int length = buffer.length();

    int sentBytes = send(fd, data, length, 0);
    if (sentBytes < 0 || sentBytes != length) {
        throw std::runtime_error("Error while sending a message to a client!");
    }
}


void Client::respondWithPrefix(const std::string &message) {
	this->sendWithLineEnding(":" + getPrefix() + " " + message);
}

void Client::sendWelcomeMessage() {
    if (status != client_state::LOGIN || user_name.empty() || real_name.empty() || nick_name.empty()) {
        return;
    }

    status = client_state::REGISTERED;
    this->respondWithPrefix(IRCResponse::RPL_WELCOME(nick_name));

    std::ostringstream welcomeMessage;
    welcomeMessage << host_name << ":" << port << " is now known as " << nick_name << ".";
    std::cout << welcomeMessage.str() << std::endl;
}


void Client::handleChannelJoin(Channel *channel) {
    if (!channel) {
        return;
    }

    channel->add_client(this);
    chan = channel;

    std::string users;
    std::vector<std::string> nicknames = chan->getNicknames();
    for (std::vector<std::string>::iterator it = nicknames.begin(); it != nicknames.end(); ++it) {
        users += *it + " ";
    }

    std::string joinedUsers = users.empty() ? "" : " " + users;
    std::string channelName = channel->getName();

    respondWithPrefix(IRCResponse::RPL_NAMREPLY(nick_name, channelName, users));
    respondWithPrefix(IRCResponse::RPL_ENDOFNAMES(nick_name, channelName));
    channel->sendAll(IRCResponse::RPL_JOIN(getPrefix(), channelName));

    std::string message = nick_name + " has joined the channel " + channelName + joinedUsers;
    std::cout << message << std::endl;
}


void Client::handleChannelLeave() {
	if (!chan)
		return;

	const std::string name = chan->getName();

	chan->sendAll(IRCResponse::RPL_PART(getPrefix(), chan->getName()));
	chan->handleClientRemoval(this);

	std::string message = nick_name + " has left the channel " + name;
	std::cout << message << std::endl;
}
