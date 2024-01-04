#include "Parser.hpp"
#include <assert.h>
#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <vector>

// Lexer.

// namespace lex_error {
// enum type {
// 	carriage_return_or_line_feed,
// 	nil,
// };
// }

// struct lexeme {
// 	enum {
// 		carriage_return_line_feed,
// 		word,

// 		error,
// 		nothing,
// 	} tag;
// 	union {
// 		char *word;
// 		lex_error::type error;
// 	} value;
// };

// struct lex_state {
// 	enum {
// 		in_word,
// 		out_of_word,
// 		carriage_return_found,
// 	} state;
// 	std::string word;
// 	bool in_trailing;
// };

lexeme lex(char c, lex_state *l) {
	if (c == 0)
		return (lexeme){
		    .tag = lexeme::error,
		    .value.error = lex_error::nil,
		};
	if (c == '\n' && l->state != lex_state::carriage_return_found) {
		return (lexeme){
		    .tag = lexeme::error,
		    .value.error = lex_error::carriage_return_or_line_feed,
		};
	}

	switch (l->state) {
	case lex_state::carriage_return_found:
		return (lexeme){.tag = lexeme::carriage_return_line_feed};
		break;
	case lex_state::in_word:
		if (l->in_trailing) {
			if (c == '\r') {
				l->state = lex_state::carriage_return_found;
				char *word = strdup(l->word.c_str());
				assert(word != 0);
				return (lexeme){
				    .tag = lexeme::word,
				    .value.word = word,
				};
			}
			l->word += c;
			return (lexeme){.tag = lexeme::nothing};
		}
		if (c == '\r') {
			return (lexeme){
			    .tag = lexeme::error,
			    .value.error =
				lex_error::carriage_return_or_line_feed,
			};
		}
		if (c == ' ') {
			l->state = lex_state::out_of_word;
			char *word = strdup(l->word.c_str());
			assert(word != 0);
			l->word = "";
			return (lexeme){
			    .tag = lexeme::word,
			    .value.word = word,
			};
		}
		l->word += c;
		return (lexeme){.tag = lexeme::nothing};
		break;
	case lex_state::out_of_word:
		if (c == ':') {
			l->in_trailing = true;
			l->state = lex_state::in_word;
			return (lexeme){.tag = lexeme::nothing};
		}
		if (c == ' ') {
			return (lexeme){.tag = lexeme::nothing};
		}
		if (c == '\r') {
			l->state = lex_state::carriage_return_found;
			return (lexeme){.tag = lexeme::nothing};
		}
		l->state = lex_state::in_word;
		l->word += c;
		return (lexeme){.tag = lexeme::nothing};
		break;
	}
	std::cout << "stex\n";
	assert(0);
}

std::vector<lexeme> lex_string(const char *string, lex_state *state) {
	std::vector<lexeme> result;

	for (int i = 0; string[i] != 0; i++) {
		char c = string[i];
		lexeme l = lex(c, state);
		if (l.tag != lexeme::nothing) {
			result.push_back(l);
		}
	}
	return result;
}

// Parser.

// struct message {
// 	char *prefix; // Nilable. Unused.
// 	char *command;
// 	char **params;
// 	int params_count;
// };

void print_message(message m) {
	std::cout << "prefix: " << m.prefix << std::endl
		  << "command: " << m.command << std::endl
		  << "params_count: " << m.params_count << std::endl;
	for (int i = 0; i < m.params_count; i++) {
		std::cout << "\t" << m.params[i] << std::endl;
	}
}

// namespace parse_error {
// enum type {
// 	no_command,
// };
// }

// struct parseme {
// 	enum parseme_tag {
// 		nothing,
// 		message,
// 		error,
// 	} tag;
// 	union {
// 		::message message;
// 		parse_error::type error;
// 	} value;
// };

// template <class t> struct optional {
// 	t value;
// 	bool has_value;
// };

// struct parse_state {
// 	optional<std::string> prefix;
// 	std::vector<std::string> words;
// };

parseme parse(lexeme l, parse_state *p) {
	switch (l.tag) {
	case lexeme::carriage_return_line_feed: {
		if (p->words.size() == 0) {
			return (parseme){
			    .tag = parseme::error,
			    .value.error = parse_error::no_command,
			};
		}
		message m = {
		    .params_count = p->words.size() - 1,
		};
		if (p->prefix.has_value) {
			m.prefix = strdup(p->prefix.value.c_str());
			assert(m.prefix != 0);
		} else {
			m.prefix = 0;
		}
		{
			m.command = strdup(p->words[0].c_str());
			assert(m.command != 0);
		}
		m.params = (char **)malloc(sizeof(*m.params) * m.params_count);
		assert(m.params != 0);
		for (unsigned long i = 0; i < p->words.size() - 1; i++) {
			m.params[i] = strdup(p->words[1 + i].c_str());
			assert(m.params[i] != 0);
		}
		p->prefix = (optional<std::string>){
		    .has_value = false,
		};
		p->words.erase(p->words.begin(), p->words.end());
		return (parseme){
		    .tag = parseme::message,
		    .value.message = m,
		};
		break;
	}
	case lexeme::word: {
		char *word = l.value.word;
		assert(strlen(word) != 0);
		if (word[0] == ':') {
			assert(p->words.size() == 0);
			char *without_colon = word + 1;
			assert(without_colon != 0);
			p->prefix = (optional<std::string>){
			    .has_value = true,
			    .value = without_colon,
			};
			free(word);
			return (parseme){.tag = parseme::nothing};
		} else {
			p->words.push_back(word);
			free(word);
			return (parseme){.tag = parseme::nothing};
		}
		break;
	}
	default:
		// To-do: handle errors.
		return (parseme){.tag = parseme::error};
	}
}

std::vector<parseme> parse_lexeme_string(std::vector<lexeme> lexemes,
					 parse_state *state) {
	std::vector<parseme> result;

	for (unsigned long i = 0; i < lexemes.size(); i++) {
		lexeme l = lexemes[i];
		parseme p = parse(l, state);
		if (p.tag != parseme::nothing) {
			result.push_back(p);
		}
	}
	return result;
}
