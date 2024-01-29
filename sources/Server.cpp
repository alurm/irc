#include "Server.hpp"
#include "Socket.hpp"

Server::Server(const std::string &port, const std::string &pass) :
	port(port),
	pass(pass),
	// Domain -- internet protocol, version 4.
	// Type -- transfer control protocol.
	// Protocol -- internet protocol (pseudo protocol).
	sock(AF_INET, SOCK_STREAM, 0)
{


	{
		int reuse_address = 1;
		// What does SO_REUSEADDR do?
		// Answer: https://www.unixguide.net/network/socketfaq/4.5.shtml.
		if (setsockopt(
			sock.value,
			SOL_SOCKET, // Level: socket level: socket.
			SO_REUSEADDR,
			&reuse_address,
			sizeof(reuse_address)
		) == -1)
			throw std::runtime_error("Error: Unable to set SO_REUSEADDR on socket.");
	}

	struct sockaddr_in serv_addr = {
		// Internet protocol, version 4.
		.sin_family = AF_INET,
		// Bind on all local interfaces.
		.sin_addr.s_addr = INADDR_ANY,
		// "htons" -- "host to two network ordered octets" (one short).
		.sin_port = htons(atoi(port.c_str())),
	};

	// Static and dynamic casts would not work here.
	// That's because there is no inheritance involved.
	if (bind(
		sock.value,
		reinterpret_cast<sockaddr *>(&serv_addr),
		sizeof(serv_addr)
	) < 0)
		throw std::runtime_error("Error: Failed to bind the socket.");

	if (listen(sock.value, SOMAXCONN) < 0)
		throw std::runtime_error("Error: Failed to start listening on the socket.");

	// File control, file set flags, non blocking.
	if (fcntl(sock.value, F_SETFL, O_NONBLOCK))
		throw std::runtime_error("Error: Unable to set socket as non-blocking.");
}

void Server::connect_client() {
	sockaddr_in addr;

	int fd;
	{
		socklen_t size = sizeof(addr);
		// Size is an out parameter to know the actual size.
		// But we know that sockaddr_in is used.
		// Therefore, this information is useless.
		fd = accept(sock.value, reinterpret_cast<sockaddr *>(&addr), &size);
		if (fd < 0) throw std::runtime_error("Error while accepting a new client!");
	}

	pollfd pfd = { .fd = fd, .events = POLLIN };
	fds.push_back(pfd);
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) throw std::runtime_error(
		"Error: setting client fd to non-blocking mode failed!"
	);
	char hostname[NI_MAXHOST];
	if (getnameinfo(reinterpret_cast<sockaddr *>(&addr), sizeof(addr),
			hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0) {
		throw std::runtime_error(
		    "Error while getting a hostname on a new client!");
	}

	Client client(fd, ntohs(addr.sin_port), std::string(hostname));
	clients.insert(std::make_pair(fd, client));

	char message[1000];
	sprintf(message, "%s:%d has connected.\n",
		client.host_name.c_str(), client.port);
	std::cout << message;
}

void Server::disconnectClient(int fd) {
	Client &client = clients.at(fd);
	client.handleChannelLeave();
	char message[1000];
	sprintf(message, "%s:%d has disconnected!",
		client.host_name.c_str(), client.port);
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

	if (bytesRead < 0 && errno != EWOULDBLOCK) {
		std::cout << "Error occurred during recv: " << strerror(errno)
			  << std::endl;
		throw std::runtime_error(
		    "Error while reading buffer from a client!");
	}
	message.append(buffer);
	// size_t pos = message.find("\r\n");

	// if (pos != std::string::npos) {
	// 	message.replace(message.rfind("\r\n"), 2, "\r\n\r\n");
	// }

	std::stringstream ss(message);
	std::string syntax;

	std::string trimmedMessage = trim(message);
	lex_state lexerState = {
	    .state = lex_state::in_word,
	    .word = "",
	    .in_trailing = false,
	};
	std::vector<lexeme> lexemes = lex_string(message.c_str(), &lexerState);
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
	}
	return m;
}

void Server::handle_client_message(int fd) {
	if (clients.count(fd) > 0) {
		;
		Client &client = clients.at(fd);
		message message = this->get_client_message(fd);
		std::vector<std::string> paramsVector;
		for (int i = 0; i < message.params_count; i++) {
			std::string param = std::string(message.params[i]);
			paramsVector.push_back(param);
		}
		if (message.command) {
			dispatch(client, message);
		}
	}
}

void Server::start() {
	// Be notified about incoming connections.
	fds.push_back((pollfd){ .fd = sock.value, .events = POLLIN });

	while (true) {
		// Timeout is -1 to wait for the first event indefinitely.
		if (poll(fds.data(), fds.size(), -1) < 0)
			throw std::runtime_error("Error while polling from fds!");

		for (
			std::vector<pollfd>::iterator it = fds.begin();
			it != fds.end();
			++it
		) {
			if (it->revents & POLLHUP) {
				disconnectClient(it->fd);
				break;
			}

			if (it->revents & POLLIN) {
				if (it->fd == sock.value) {
					connect_client();
					break;
				}
			}

			handle_client_message(it->fd);
		}
		// system("leaks ircserv");
	}
}

Client *Server::getClient(const std::string &nickname) {
	for (std::map<int, Client>::iterator it = clients.begin();
	     it != clients.end(); ++it) {
		if (nickname == it->second.nick_name) {
			return &it->second;
		}
	}
	return NULL;
}

Channel *Server::getChannel(const std::string &name) {
	for (size_t i = 0; i < channels.size(); ++i) {
		Channel *currentChannel = channels[i];
		if (currentChannel && name == currentChannel->name) {
			return currentChannel;
		}
	}
	return NULL;
}

Channel *Server::addChannel(const std::string &name, const std::string &key,
			    Client &client) {
	Channel *channel = new Channel(name, key, client);
	channels.push_back(channel);

	return channel;
}

void Server::dispatch(Client &c, message m) {

	std::vector<std::string> args;

	Base2 *command = NULL;

	for (int i = 0; i < m.params_count; ++i) {
		args.push_back(std::string(m.params[i]));
		delete[] m.params[i];
	}

	delete[] m.params;
	m.params = NULL;

	if (strcmp(m.command, "PASS") == 0) {
		std::cout << "in pass\n";
		command = new Pass(this, false);
	} else if (strcmp(m.command, "JOIN") == 0) {
		command = new Join(this, true);
	} else if (strcmp(m.command, "NICK") == 0) {
		command = new Nick(this, false);
	} else if (strcmp(m.command, "USER") == 0) {
		command = new User(this, false);
	} else if (strcmp(m.command, "QUIT") == 0) {
		command = new Quit(this, false);
	} else if (strcmp(m.command, "MODE") == 0) {
		command = new Mode(this, true);
	} else if (strcmp(m.command, "TOPIC") == 0) {
		command = new Topic(this, true);
	} else if (strcmp(m.command, "PING") == 0) {
		command = new Ping(this, true);
	} else if (strcmp(m.command, "PRIVMSG") == 0) {
		command = new PrivMsg(this, true);
	} else if (strcmp(m.command, "PONG") == 0) {
		command = new Pong(this, true);
	} else if (strcmp(m.command, "KICK") == 0) {
		command = new Kick(this, true);
	} else if (strcmp(m.command, "INVITE") == 0) {
		command = new Invite(this, true);
	} else if (strcmp(m.command, "PART") == 0) {
		command = new Part(this, true);
	} else if (strcmp(m.command, "WHO") == 0) {
		command = new Who(this, true);
	} else if (strcmp(m.command, "")) {
		return;
	} else {
		c.respondWithPrefix(IRCResponse::ERR_UNKNOWNCOMMAND(
		    c.nick_name, m.command));
		return;
	}

	// Buggy?
	if (c.status != client_state::REGISTERED && command->isAuthenticationRequired()) {
		c.respondWithPrefix(
		    IRCResponse::ERR_NOTREGISTERED(c.nick_name));
		if (command != NULL) {
			delete command;
		}
		return;
	}
	if (command != NULL) {
		command->execute(c, args);
		delete command;
	}
}

// Not very efficient.
void Server::updateNicknameInClients(int fd, const std::string &newNickname) {
	std::map<int, Client>::iterator clientIt = clients.find(fd);
	if (clientIt != clients.end()) {
		clientIt->second.nick_name = newNickname;
	}
}

void Server::updateNicknameInChannels(const std::string &oldNickname,
				      const std::string &newNickname) {
	for (size_t i = 0; i < channels.size(); ++i) {
		std::vector<Client *> channelClients =
		    channels[i]->clients;
		std::vector<Client *> channelOps = channels[i]->operators;

		for (size_t j = 0; j < channelClients.size(); ++j) {
			if (channelClients[j]->nick_name == oldNickname) {
				channelClients[j]->nick_name = newNickname;
			}
		}

		for (size_t j = 0; j < channelOps.size(); ++j) {
			if (channelOps[j]->nick_name == oldNickname) {
				channelOps[j]->nick_name = newNickname;
			}
		}
	}
}
