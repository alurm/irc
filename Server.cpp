#include "Server.hpp"

Server::Server(const std::string &port, const std::string &pass)
    : port(port), host("127.0.0.1"), pass(pass) {
	running = 1;
	sock = initializeSocket();
}

int Server::initializeSocket() {
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		throw std::runtime_error("Eror: Unabl to open the socket 🤮");
	}

	int optval = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval,
		       sizeof(optval))) {
		throw std::runtime_error(
		    "Error: Unable to set socket options.");
	}

	if (fcntl(sock_fd, F_SETFL, O_NONBLOCK)) {
		throw std::runtime_error(
		    "Error: Unable to set socket as non-blocking.");
	}

	struct sockaddr_in serv_addr = {};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(atoi(port.c_str()));

	if (bind(sock_fd, reinterpret_cast<sockaddr *>(&serv_addr),
		 sizeof(serv_addr)) < 0) {
		throw std::runtime_error("Error: Failed to bind the socket.");
	}

	if (listen(sock_fd, MAX_CLIENTS) < 0) {
		throw std::runtime_error(
		    "Error: Failed to start listening on the socket.");
	}

	return sock_fd;
}

Server::~Server() {
	for (size_t i = 0; i < channels.size(); i++)
		delete clients[i];
}

void Server::connect_client() {
	sockaddr_in addr = {};
	socklen_t size = sizeof(addr);

	int fd = accept(sock, reinterpret_cast<sockaddr *>(&addr), &size);
	if (fd < 0) {
		throw std::runtime_error("Error while accepting a new client!");
	}

	pollfd pfd = {fd, POLLIN, 0};
	fds.push_back(pfd);

	char hostname[NI_MAXHOST];
	if (getnameinfo(reinterpret_cast<sockaddr *>(&addr), sizeof(addr),
			hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0) {
		throw std::runtime_error(
		    "Error while getting a hostname on a new client!");
	}

	Client *client =
	    new Client(fd, ntohs(addr.sin_port), std::string(hostname));
	clients.insert(std::make_pair(fd, client));

	char message[1000];
	sprintf(message, "%s:%d has connected.\n",
		client->getHostname().c_str(), client->getPort());
	std::cout << message;
}

void Server::disconnectClient(int fd) {
	try {
		Client *client = clients.at(fd);
		// client->handleChannelLeave();
		char message[1000];
		sprintf(message, "%s:%d has disconnected!",
			client->getHostname().c_str(), client->getPort());
		std::cout << message << std::endl;
		clients.erase(fd);
		std::vector<pollfd>::iterator it_b = fds.begin();
		std::vector<pollfd>::iterator it_e = fds.end();
		while (it_b != it_e) {
			if (it_b->fd == fd) {
				it_b = fds.erase(it_b);
				close(fd);
				break;
			} else {
				++it_b;
			}
		}

		delete client;
	} catch (const std::exception &e) {
		std::cout << "Error while disconnecting! " << e.what()
			  << std::endl;
	}
}

struct message Server::get_client_message(int fd) {
	struct message m;
	std::string message;
	char buffer[1024];
	int bytesRead;

	bytesRead = recv(fd, buffer, sizeof(buffer), 0);

	if (bytesRead < 0 && errno != EWOULDBLOCK) {
		std::cout << "Error occurred during recv: " << strerror(errno)
			  << std::endl;
		throw std::runtime_error(
		    "Error while reading buffer from a client!");
	}
	message.append(buffer);
	std::cout << "message is " << message << std::endl;
	std::stringstream ss(message);
	std::string syntax;

	std::cout << "bull" << (message.back() == '\n') << std::endl;
	size_t lastNewlinePos = message.find_last_of('\n');
	if (lastNewlinePos != std::string::npos) {
		message.replace(lastNewlinePos, 1, "\r\n");
	}
	std::cout << "Modified message is " << message << std::endl;
	std::string simulated_message =
	    ":Nickname!username@hostname.com PRIVMSG #channel :Hello everyone! "
	    "How are you today?\r\n";
	lex_state lexerState = {
	    .state = lex_state::in_word,
	    .word = "",
	    .in_trailing = false,
	};
	std::vector<lexeme> lexemes = lex_string(message.c_str(), &lexerState);
	if (lexemes.empty()) {
		std::cout << "lexemes are empty\n";
	}
	parse_state parserState = {
	    .prefix =
		{
		    .has_value = false,
		},
	};
	std::vector<parseme> parsedMessages =
	    parse_lexeme_string(lexemes, &parserState);
	if (!parsedMessages.empty()) {
		for (size_t i = 0; i < parsedMessages.size(); ++i) {
			if (parsedMessages[i].tag == parseme::message) {
				m = parsedMessages[i].value.message;
			}
		}
	} else {
		std::cout << "parsedMessages is empty!" << std::endl;
	}
	return m;
}

void Server::handle_client_message(int fd) {
	std::cout << "In handle client message\n";
	try {
		Client *client = clients.at(fd);
		(void)client;
		message message = this->get_client_message(fd);
		std::cout << "?????\n";
		// std::cout << "prefix ->> " << message.prefix << std::endl;
		std::cout << "command ->> " << message.command << std::endl;

		if (message.params != nullptr) {
			std::cout << "params ->> " << message.params[0]
				  << std::endl;
		}
		/*
			TODO dispatch(message)
			.validation for each message
			.execute
			.rsponce
		*/
		std::vector<std::string> paramsVector;
		// Iterate through the char** array and convert each char* to
		// std::string
		for (int i = 0; i < message.params_count; i++) {
			std::cout << "in loop \n";
			std::string param = std::string(message.params[i]);
			paramsVector.push_back(param);
		}
		std::cout << "here >>>>>> \n";
		dispatch(client, message);
	} catch (const std::exception &e) {
		std::cout << "Error while handling the client message! "
			  << e.what() << std::endl;
	}
}

void Server::start() {
	pollfd srv = {sock, POLLIN, 0};
	fds.push_back(srv);

	std::cout << "Server is running...\n";
	std::vector<pollfd>::iterator it;

	while (running) {
		if (poll(&fds[0], fds.size(), -1) < 0) {
			throw std::runtime_error(
			    "Error while polling from fd!");
		}

		for (it = fds.begin(); it != fds.end(); ++it) {
			if (it->revents == 0) {
				continue;
			}

			if (it->revents & POLLHUP) {
				disconnectClient(it->fd);
				break;
			}

			if (it->revents & POLLIN) {
				if (it->fd == sock) {
					connect_client();
					break;
				}
			}
			handle_client_message(it->fd);
		}
	}
}

std::string Server::getPassword() const { return pass; }

Client *Server::getClient(const std::string &nickname) {
	for (std::map<int, Client *>::iterator it = clients.begin();
	     it != clients.end(); ++it) {
		if (it->second && nickname == it->second->getNickname()) {
			return it->second;
		}
	}
	return NULL;
}

Channel *Server::getChannel(const std::string &name) {
	for (size_t i = 0; i < channels.size(); ++i) {
		Channel *currentChannel = channels[i];
		if (currentChannel && name == currentChannel->getName()) {
			return currentChannel;
		}
	}
	return NULL;
}

Channel *Server::addChannel(const std::string &name, const std::string &key,
			    Client *client) {
	Channel *channel = new Channel(name, key, client);
	channels.push_back(channel);

	return channel;
}

void Server::dispatch( Client *c, message m) {
	std::cout << " In dispatch\n";
	std::vector<std::string> args;
	std::cout << "222222222 \n";

	for (int i = 0; i < m.params_count; ++i) {
		args.push_back(std::string(m.params[i]));
	}
	std::cout << "333333333 \n";

	if (strcmp(m.command, "PASS") == 0) {
		Pass* pass = new Pass(this, false);
		std::cout << "in pass\n";
		pass->execute(c, args);
	}

}
