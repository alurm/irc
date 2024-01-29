#ifndef SOCKET_HPP
#define SOCKET_HPP

struct Socket {
	int value;
	Socket(int domain, int type, int protocol);
	~Socket();
};

#endif
