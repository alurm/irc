#include "Server.hpp"
#include "IRCResponse.hpp"

void master(char *port, char *password) {
	Server server(port, password);
	server.start();
}

int main(int argc, char **argv) {
	try {
		if(argc != 3) throw std::runtime_error("Usage: ./ircserv <port> <password>");
		master(argv[1], argv[2]);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}
