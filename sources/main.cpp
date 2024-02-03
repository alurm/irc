#include "Server.hpp"

int main(int argc, char **argv) {
	try {
		if(argc != 3) throw std::runtime_error("Usage: ./ircserv <port> <password>");
		if (!is_valid_args(argv[1], argv[2]))
		{
			return 1;
		}
		Server server(argv[1], argv[2]);
		server.start();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}
