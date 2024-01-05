#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Channel.hpp"
#include <iostream>
#include <map>
#include "IRCResponse.hpp"
#include "State.hpp"
#include <sstream>
class Channel;

class Client {
      private:
	int fd;
	int port;

	std::string nick_name;
	std::string user_name;
	std::string host_name;
	std::string real_name;

	Channel *chan;
	client_state::ClientState status;

      public:
	Client(int fd, int port, const std::string &hostname);
	~Client();
	// get
	int getFd() const;
	int getPort() const;
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getRealname() const;
	std::string getHostname() const;
	std::string getPrefix() const;
	Channel *getChannel() const;

	// set
	void setNickname(const std::string &nickname);
	void setUsername(const std::string &username);
	void setRealname(const std::string &realname);
	void setState(client_state::ClientState state);
	void setChannel(Channel *channel);

	// some mechanism to state knowing
	bool isInRegisteredState() const;


	/* Send/Recieve Actions */
	void sendWithLineEnding(const std::string &message) const;
	void respondWithPrefix(const std::string &message);
	void sendWelcomeMessage();

	/* Client Actions */

	void handleChannelJoin(Channel *channel);
	void handleChannelLeave();
};

#endif 