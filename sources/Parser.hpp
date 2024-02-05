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

struct parsing_error : std::exception {};

// Lexer.

struct lexeme {
	enum {
		terminator,
		word,
	} tag;
	struct {
		std::string word;
	} value;
};

struct lex_state {
	enum {
		in_word,
		out_of_word,
	} state;
	std::string word;
	bool in_trailing;

	lex_state() {
		in_trailing = false;
		word = "";
		state = in_word;
	}
};

struct message {
	optional<std::string> prefix;
	std::string command;
	std::vector<std::string> params;
};

struct parse_state {
	optional<std::string> prefix;
	std::vector<std::string> words;
};

std::vector<lexeme> lex(char c, lex_state &l);
std::vector<lexeme> lex_string(std::string string, lex_state &state);
void print_message(message m);
optional<message> parse(lexeme l, parse_state &p);
std::vector<message> parse_lexeme_string(std::vector<lexeme> lexemes,
					 parse_state &state);
