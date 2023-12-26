#include "Client.hpp"

Client::Client(int fd, int port, const std::string &hostname)
    : fd(fd), port(port), hostname(hostname)
{

}

int	Client::get_port() const { 
	return port;
}
std::string	Client::get_hostname() const { 
	return hostname; 
}

Client::~Client() {}