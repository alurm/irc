#include "Server.hpp"
#include "IRCResponse.hpp"

int main(int argc, char **argv) {
	try {

		if(argc != 3) throw std::runtime_error("Usage: ./ircserv <port> <password>");
		Server server(argv[1], argv[2]);
		server.start();

	} catch (std::exception &e) {

		std::cerr << e.what() << std::endl;
		exit(1);

	}
}