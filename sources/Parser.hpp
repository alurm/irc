#pragma once

#include <assert.h>
#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdexcept>

template <class t> class optional {
	t value;

public:

	bool has_value;

	optional(t v) {
		has_value = true;
		value = v;
	}

	optional() {
		has_value = false;
	}

	t operator()() {
		if (!has_value) throw std::runtime_error("Optional is empty!");
		return value;
	}
};

// Lexer.

namespace lex_error {
enum type {
	carriage_return_or_line_feed,
	nil,
};
}

struct lexeme {
	enum {
		carriage_return_line_feed,
		word,

		error,
		nothing,
	} tag;
	struct {
		std::string word;
		lex_error::type error;
	} value;
};

struct lex_state {
	enum {
		in_word,
		out_of_word,
		carriage_return_found,
	} state;
	std::string word;
	bool in_trailing;
};

struct message {
	optional<std::string> prefix;
	std::string command;
	std::vector<std::string> params;
};

namespace parse_error {
enum type {
	no_command,
};
}

struct parseme {
	enum parseme_tag {
		nothing,
		message,
		error,
	} tag;
	struct {
		::message message;
		parse_error::type error;
	} value;
};

struct parse_state {
	optional<std::string> prefix;
	std::vector<std::string> words;
};

lexeme lex(char c, lex_state *l);
std::vector<lexeme> lex_string(std::string string, lex_state *state);
void print_message(message m);
parseme parse(lexeme l, parse_state &p);
std::vector<parseme> parse_lexeme_string(std::vector<lexeme> lexemes,
					 parse_state &state);
