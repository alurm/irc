#include "Server.hpp"

Server::Server(const std::string &port, const std::string &pass)
    : port(port), host("127.0.0.1"), pass(pass) {
	sock = initializeSocket();
}

int Server::initializeSocket() {
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0) {
		throw std::runtime_error("Eror: Unabl to open the socket!");
	}

	int optval = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval,
		       sizeof(optval)) == -1) {
		close(sock_fd);
		throw std::runtime_error(
		    "Error: Unable to set socket options.");
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

	if (fcntl(sock_fd, F_SETFL, O_NONBLOCK)) {
		throw std::runtime_error(
		    "Error: Unable to set socket as non-blocking.");
	}
	this->clients.insert(std::pair<int, Client *>(sock, NULL));

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
		close(sock);
		throw std::runtime_error("Error while accepting a new client!");
	}

	pollfd pfd = {fd, POLLIN, 0};
	fds.push_back(pfd);
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		close(sock);
		throw std::runtime_error(
		    "Error: setting client fd to non-blocking mode failed!");
	}
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
		client->handleChannelLeave();
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
	m.prefix = NULL;
	m.command = NULL;
	m.params = NULL;
	m.params_count = 0;
	std::string message;
	char buffer[1024];
	int bytesRead;

	bytesRead = recv(fd, buffer, sizeof(buffer), 0);
	// while ((bytesRead = recv(fd, buffer, sizeof(buffer), 0)) > 0) {
	// 	message.append(buffer, bytesRead);
	// 	if (message.find("\r\n") != std::string::npos) {
	// 		std::cout << "🇨🇿\n";
	// 		break;
	// 	}
	// }

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
	// size_t lastNewlinePos = message.find_last_of('\n');
	// if (lastNewlinePos != std::string::npos) {
	// 	message.replace(lastNewlinePos, 1, "\r\n");
	// }
	std::cout << "Modified message is " << message << std::endl;
	std::string trimmedMessage = trim(message);
	lex_state lexerState = {
	    .state = lex_state::in_word,
	    .word = "",
	    .in_trailing = false,
	};
	std::vector<lexeme> lexemes =
	    lex_string(trimmedMessage.c_str(), &lexerState);
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
				std::cout << "command is " << m.command
					  << std::endl;
			}
		}
	} else {
		std::cout << "parsedMessages is empty!" << std::endl;
	}
	return m;
}

void Server::handle_client_message(int fd) {
	try {
		if (clients.count(fd) > 0) {
			Client *client = clients.at(fd);
			(void)client;
			message message = this->get_client_message(fd);
			std::vector<std::string> paramsVector;
			for (int i = 0; i < message.params_count; i++) {
				std::string param =
				    std::string(message.params[i]);

				std::cout << "Param in index " << i << "is --->"
					  << param << std::endl;
				paramsVector.push_back(param);
			}
			if (message.command) {
				dispatch(client, message);
			}
		}
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

	while (1) {
		if (poll(&fds[0], fds.size(), -1) < 0) {
			throw std::runtime_error(
			    "Error while polling from fd!");
		}

		for (it = fds.begin(); it != fds.end(); ++it) {
			if (it->revents == 0) {
				continue;
			}

			if ((it->revents & POLLHUP) == POLLHUP) {
				disconnectClient(it->fd);
				break;
			}

			if ((it->revents & POLLIN) == POLLIN) {
				if (it->fd == sock) {
					connect_client();
					break;
				}
			}
			handle_client_message(it->fd);
		}
	}
	closeFreeALL();
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

void Server::dispatch(Client *c, message m) {

	std::cout << "Status is " << c->status << std::endl;
	std::cout << " In dispatch\n";
	std::vector<std::string> args;

	Base2 *command = NULL;

	for (int i = 0; i < m.params_count; ++i) {
		args.push_back(std::string(m.params[i]));
		delete[] m.params[i];
	}

	delete[] m.params;
	m.params = NULL;

	if (strcmp(m.command, "PASS") == 0) {
		command = new Pass(this, false);
		std::cout << "in pass\n";
	} else if (strcmp(m.command, "JOIN") == 0) {
		command = new Join(this, true);
		std::cout << "in join\n";
	} else if (strcmp(m.command, "NICK") == 0) {
		command = new Nick(this, false);
		std::cout << "in nick\n";
	} else if (strcmp(m.command, "USER") == 0) {
		command = new User(this, false);
		std::cout << "in user\n";
	} else if (strcmp(m.command, "QUIT") == 0) {
		command = new Quit(this, false);
		std::cout << "in quit\n";
	} else if (strcmp(m.command, "MODE") == 0) {
		command = new Mode(this, true);
		std::cout << "in mode\n";
	} else if (strcmp(m.command, "TOPIC") == 0) {
		command = new Topic(this, true);
		std::cout << "in topic\n";
	} else if (strcmp(m.command, "PING") == 0) {
		command = new Ping(this, true);
		std::cout << "in ping\n";
	} else if (strcmp(m.command, "PRIVMSG") == 0) {
		command = new PrivMsg(this, true);
		std::cout << "in priv_msg\n";
	} else if (strcmp(m.command, "PONG") == 0) {
		command = new Pong(this, true);
		std::cout << "in pong\n";
	} else if (strcmp(m.command, "KICK") == 0) {
		command = new Kick(this, true);
		std::cout << "in kick\n";
	} else if (strcmp(m.command, "INVITE") == 0) {
		command = new Invite(this, true);
		std::cout << "in invite\n";
	} else if (strcmp(m.command, "PART") == 0) {
		command = new Part(this, true);
		std::cout << "in invite\n";
	} else if (strcmp(m.command, "WHO") == 0) {
		command = new Who(this, true);
		std::cout << "in who\n";
	} else if (strcmp(m.command, "")) {
		std::cout << "empty\n";
		return;
	} else {
		c->respondWithPrefix(IRCResponse::ERR_UNKNOWNCOMMAND(
		    c->getNickname(), m.command));
		return;
	}

	if (!c->isInRegisteredState() && command->isAuthenticationRequired()) {
		c->respondWithPrefix(
		    IRCResponse::ERR_NOTREGISTERED(c->getNickname()));
		if (command != NULL) {
			delete command;
		}
		return;
	}
	if (command != NULL) {
		std::cout << "before execute\n";
		command->execute(c, args);
		delete command;
	}
}

void Server::closeFreeALL(void) {
	std::map<int, Client *>::iterator it = clients.begin();
	for (; it != clients.end(); ++it) {
		close(it->first);
		if (it->second)
			delete it->second;
	}
	clients.clear();
}

void Server::updateNicknameInClients(int fd, const std::string &newNickname) {
	std::map<int, Client *>::iterator clientIt = clients.find(fd);

	if (clientIt != clients.end()) {
		clientIt->second->setNickname(newNickname);
	}
}

void Server::updateNicknameInChannels(const std::string &oldNickname,
				      const std::string &newNickname) {
	for (size_t i = 0; i < channels.size(); ++i) {
		std::vector<Client *> channelClients =
		    channels[i]->getClients();
		std::vector<Client *> channelOps = channels[i]->getOperators();

		for (size_t j = 0; j < channelClients.size(); ++j) {
			if (channelClients[j]->getNickname() == oldNickname) {
				channelClients[j]->setNickname(newNickname);
			}
		}

		for (size_t j = 0; j < channelOps.size(); ++j) {
			if (channelOps[j]->getNickname() == oldNickname) {
				channelOps[j]->setNickname(newNickname);
			}
		}
	}
}

std::vector<Channel *> Server::getChannels () {
	return channels;
}