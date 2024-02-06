#include "Parser.hpp"
#include <assert.h>
#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <vector>

template <class t> std::vector<t> singleton_vector(t v) {
	std::vector<t> result;
	result.push_back(v);
	return result;
}

template <class t> std::vector<t> empty_vector() {
	std::vector<t> result;
	return result;
}

template <class t> std::vector<t> pair(t v1, t v2) {
	std::vector<t> result;
	result.push_back(v1);
	result.push_back(v2);
	return result;
}

lexeme terminator = {
    .tag = lexeme::terminator,
};

std::vector<lexeme> lex(char c, lex_state &l) {
	if (c == '\r')
		return std::vector<lexeme>();

	if (c == 0)
		throw parsing_error();

	switch (l.state) {
	case lex_state::in_word:
		if (c == '\n') {
			if (l.word.size() == 0 && !l.in_trailing) {
				return empty_vector<lexeme>();
			} else {
				std::string word = l.word;
				l = lex_state();
				return pair<lexeme>(
					(lexeme){
					.tag = lexeme::word,
					.value =
						{
						.word = word,
						},
					},
					terminator
				);
			}
		} else if (l.in_trailing) {
			l.word += c;
			return empty_vector<lexeme>();
		} else if (c == ' ') {
			l.state = lex_state::out_of_word;
			std::string word = l.word;
			l.word = "";
			return singleton_vector<lexeme>((lexeme){
			    .tag = lexeme::word,
			    .value =
				{
				    .word = word,
				},
			});
		} else {
			l.word += c;
			return empty_vector<lexeme>();
		}
		break;
	case lex_state::out_of_word:
		if (c == '\n')
			return singleton_vector<lexeme>(terminator);
		else if (c == ':') {
			l.in_trailing = true;
			l.state = lex_state::in_word;
			return empty_vector<lexeme>();
		} else if (c == ' ') {
			return empty_vector<lexeme>();
		} else {
			l.word += c;
			l.state = lex_state::in_word;
			return empty_vector<lexeme>();
		}
		break;
	}
	assert(0);
}

std::vector<lexeme> lex_string(std::string string, lex_state &state) {
	std::vector<lexeme> result;

	for (int i = 0; string[i] != 0; i++) {
		char c = string[i];
		std::vector<lexeme> l = lex(c, state);
		for (std::vector<lexeme>::iterator it = l.begin();
		     it != l.end(); it++)
			result.push_back(*it);
	}
	return result;
}

void print_message(message m) {
	std::cout << "prefix: " << (m.prefix.has_value ? m.prefix() : "")
		  << std::endl
		  << "command: " << m.command << std::endl
		  << "params_count: " << m.params.size() << std::endl;
	for (size_t i = 0; i < m.params.size(); i++) {
		std::cout << "\t" << m.params[i] << std::endl;
	}
}

optional<message> parse(lexeme l, parse_state &p) {
	switch (l.tag) {
	case lexeme::terminator: {
		// if (p.words.size() == 0) throw parsing_error();

		message m = {
		    .prefix = p.prefix,
		    .command = p.words[0],
		    .params = p.words,
		};

		m.params.erase(m.params.begin());

		p = parse_state();

		return optional<message>(m);
		break;
	}
	case lexeme::word: {
		std::string word = l.value.word;

		if (word[0] == ':' && p.words.size() == 0) {
			std::string without_colon = word.substr(1);
			p.prefix = optional<std::string>(without_colon);
		} else {
			p.words.push_back(word);
		}

		return optional<message>();
	}
	default:
		throw parsing_error();
	}
}

std::vector<message> parse_lexeme_string(std::vector<lexeme> lexemes,
					 parse_state &state) {
	std::vector<message> result;

	for (size_t i = 0; i < lexemes.size(); i++) {
		lexeme l = lexemes[i];
		optional<message> p = parse(l, state);
		if (p.has_value) {
			result.push_back(p());
		}
	}
	return result;
}
