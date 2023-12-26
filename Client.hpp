#pragma once

#include <map>
#include <iostream>

class Client {
	private:    
        int             fd;
        int             port;
		std::string     hostname;
	    std::map<int,	Client *> clients;
	public:
		Client(int fd, int port, const std::string &hostname);
		int	get_port() const;
		std::string	get_hostname() const;
        ~Client();
};