#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Channel.hpp"
#include <iostream>
#include <map>
#include "IRCResponse.hpp"
#include "State.hpp"
#include <sstream>
#include "Parser.hpp"

struct Channel;

struct Client {
	int fd;
	int port;

	parse_state parser;
	lex_state lexer;

	std::string input_buffer;
	size_t input_buffer_offset;

	std::string output_buffer;
	size_t output_buffer_offset;

	enum {
		reading,
		writing,
	} state;

	std::string nick_name;
	std::string user_name;
	std::string host_name;
	std::string real_name;

	Channel *chan;
 	client_state::ClientState status;
	Client(int fd, int port, const std::string &hostname);
	~Client();

	std::string getPrefix() const;

	/* Send and receive */

	void sendWithLineEnding(const std::string &message);
	void respondWithPrefix(const std::string &message);
	void sendWelcomeMessage();

	optional<message> optional_message();

	/* Client Actions */

	void handleChannelJoin(Channel *channel);
	void handleChannelLeave();
	bool nickIsCorrect(std::string buffer);
};

#endif