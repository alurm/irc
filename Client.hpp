#pragma once

#include <map>
#include <iostream>
#include "Channel.hpp"
#include "IRCResponse.hpp"

namespace client_state {
    enum ClientState
    {
        HANDSHAKE,
        LOGIN,
        REGISTERED,
        DISCONNECTED
    } state;
}


class Client {
	private:    
        int             fd;
        int             port;

		std::string     nickname;
		std::string     username;
		std::string     hostname;
		std::string     realname;

 		Channel*        channel
		client_state::ClientState     state;
	public:
		Client(int fd, int port, const std::string &hostname);
		~Client();

		//TODO write other getters
		int	get_port() const;
		//maybe setters
		std::string	get_hostname() const;
		//some mechanism to state knowing 
		/**/
};