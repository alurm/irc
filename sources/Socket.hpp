#ifndef SOCKET_HPP
#define SOCKET_HPP

int my_close(int fd);

struct Socket {
	int value;
	Socket(int domain, int type, int protocol);
	~Socket();
};

#endif
