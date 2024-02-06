#include "Server.hpp"
#include "Socket.hpp"

Server::Server(const std::string &port, const std::string &pass)
    : port(port), pass(pass),
      // Domain -- internet protocol, version 4.
      // Type -- transfer control protocol.
      // Protocol -- internet protocol (pseudo protocol).
      sock(AF_INET, SOCK_STREAM, 0) {

	{
		int reuse_address = 1;
		// What does SO_REUSEADDR do?
		// Answer:
		// https://www.unixguide.net/network/socketfaq/4.5.shtml.
		if (setsockopt(sock.value,
			       SOL_SOCKET, // Level: socket level: socket.
			       SO_REUSEADDR, &reuse_address,
			       sizeof(reuse_address)) == -1)
			throw std::runtime_error(
			    "Error: Unable to set SO_REUSEADDR on socket.");
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
	if (bind(sock.value, reinterpret_cast<sockaddr *>(&serv_addr),
		 sizeof(serv_addr)) < 0)
		throw std::runtime_error("Error: Failed to bind the socket.");

	if (listen(sock.value, SOMAXCONN) < 0)
		throw std::runtime_error(
		    "Error: Failed to start listening on the socket.");

	// File control, file set flags, non blocking.
	if (fcntl(sock.value, F_SETFL, O_NONBLOCK))
		throw std::runtime_error(
		    "Error: Unable to set socket as non-blocking.");
}

void Server::connect_client() {
	sockaddr_in addr;

	int fd;
	{
		socklen_t size = sizeof(addr);
		// Size is an out parameter to know the actual size.
		// But we know that sockaddr_in is used.
		// Therefore, this information is useless.
		fd = accept(sock.value, reinterpret_cast<sockaddr *>(&addr),
			    &size);
		if (fd < 0)
			throw std::runtime_error(
			    "Error while accepting a new client!");
	}

	pollfd pfd = {.fd = fd, .events = POLLIN | POLLOUT};
	fds.push_back(pfd);
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error(
		    "Error: setting client fd to non-blocking mode failed!");
	char hostname[NI_MAXHOST];
	if (getnameinfo(reinterpret_cast<sockaddr *>(&addr), sizeof(addr),
			hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0) {
		throw std::runtime_error(
		    "Error while getting a hostname on a new client!");
	}

	Client client(fd, ntohs(addr.sin_port), std::string(hostname));
	clients.insert(std::make_pair(fd, client));

	char message[1000];
	sprintf(message, "%s:%d has connected.\n", client.host_name.c_str(),
		client.port);
	std::cout << message;
}

int my_close(int fd) {
	int result = close(fd);
	if (result == -1) throw std::runtime_error("Close failed!");
	return result;
}

void Server::disconnectClient(int fd) {
	Client &client = clients.at(fd);
	client.handleChannelLeave();
	char message[1000];
	sprintf(message, "%s:%d has disconnected!", client.host_name.c_str(),
		client.port);
	std::cout << message << std::endl;
	clients.erase(fd);
	std::vector<pollfd>::iterator it_b = fds.begin();
	std::vector<pollfd>::iterator it_e = fds.end();
	while (it_b != it_e) {
		if (it_b->fd == fd) {
			it_b = fds.erase(it_b);
			my_close(fd);
			break;
		} else {
			++it_b;
		}
	}
}

void Server::handle_client_message(int fd) {
	Client &client = clients.at(fd);
	try {
		optional<message> optional_message = clients.at(fd).optional_message();
		if (optional_message.has_value) {
			dispatch(client, optional_message());
		}
	} catch (parsing_error) {
		disconnectClient(fd);
		throw pollfd_iterator_invalidated();
	}
}

void Server::start() {
	// Be notified about incoming connections.
	fds.push_back((pollfd){.fd = sock.value, .events = POLLIN});

	// Retrying is not very efficient.
	while (true) {
		// Timeout is -1 to wait for the first event indefinitely.
		if (poll(fds.data(), fds.size(), -1) < 0)
			throw std::runtime_error(
			    "Error while polling from fds!");

		while (true) {
			try {
				for (std::vector<pollfd>::iterator it =
					 fds.begin();
				     it != fds.end(); ++it)
				{
					if (it->revents & POLLHUP) {
						it->revents &= ~POLLHUP;
						disconnectClient(it->fd);
						throw pollfd_iterator_invalidated();
					}

					if (it->revents & POLLIN) {
						it->revents &= ~POLLIN;

						if (it->fd == sock.value) {
							connect_client();
							throw pollfd_iterator_invalidated();
						}

						Client &c = clients.at(it->fd);

						if (c.state == Client::reading) handle_client_message(it->fd);
					}

					if (it->revents & POLLOUT) {
						it->revents &= ~POLLOUT;

						int fd = it->fd;
						Client &c = clients.at(fd);

						if (c.state == Client::writing) {
							int sentBytes = send(fd, c.output_buffer.c_str(), c.output_buffer.size(), 0);

							if (sentBytes < 0) {
								disconnectClient(fd);
								throw pollfd_iterator_invalidated();
							}

							c.output_buffer_offset += sentBytes;
							if (c.output_buffer_offset == c.output_buffer.size()) {
								c.output_buffer = std::string();
								c.output_buffer_offset = 0;
								c.state = Client::reading;
							}

						}
					}
				}
				break;
			} catch (pollfd_iterator_invalidated) {
			}
		}
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
	Command *command = NULL;

	//  << "Command is " << m.command << std::endl;
	if (m.command == "PASS") {
		command = new Pass(this, false);
	} else if (m.command == "JOIN") {
		command = new Join(this, true);
	} else if (m.command == "NICK") {
		command = new Nick(this, false);
	} else if (m.command == "USER") {
		command = new User(this, false);
	} else if (m.command == "QUIT") {
		Quit quit = Quit(this, false);
		quit.execute(c, m.params);
		return;
	} else if (m.command == "MODE") {  
		command = new Mode(this, true);
	} else if (m.command == "TOPIC") {
		command = new Topic(this, true);
	} else if (m.command == "PING") {
		command = new Ping(this, false);
	} else if (m.command == "PRIVMSG") {
		command = new PrivMsg(this, true);
	} else if (m.command == "PONG") {
		command = new Pong(this, false);
	} else if (m.command == "KICK") {
		command = new Kick(this, true);
	} else if (m.command == "INVITE") {
		command = new Invite(this, true);
	} else if (m.command == "PART") {
		command = new Part(this, true);
	} else if (m.command == "WHO") {
		command = new Who(this, true);
	} else if (m.command == "CAP") {
		command = new Cap(this, false);
	} else if (m.command == "") {
		return;
	} else {
		c.respondWithPrefix(
		    IRCResponse::ERR_UNKNOWNCOMMAND(c.nick_name, m.command));
		return;
	}

	if (c.status != client_state::REGISTERED &&
	    command->isAuthenticationRequired()) {
		c.respondWithPrefix(
		    IRCResponse::ERR_NOTREGISTERED(c.nick_name));
		delete command;
		return;
	}
	if (command != NULL) {
		command->execute(c, m.params);
	}

	delete command;
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
		std::vector<Client *> channelClients = channels[i]->clients;
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

Server::~Server() {

	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++) {
		delete *it;
	}
}