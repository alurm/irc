#include "Socket.hpp"
#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>

Socket::Socket(int domain, int type, int protocol) {
	value = socket(domain, type, protocol);
	if (value < 0) throw std::runtime_error("Wasn't able to open a socket");
}

Socket::~Socket() {
	my_close(value);
}
