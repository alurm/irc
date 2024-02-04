#include "Parser.hpp"
#include <assert.h>
#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <string>
#include <vector>

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
		if (c == '\r') {
			if (l->word.empty()) {
				l->state = lex_state::carriage_return_found;
				return (lexeme){.tag = lexeme::nothing};
			}

			char *word = strdup(l->word.c_str());
			assert(word != 0);
			l->word = "";
			l->state = lex_state::carriage_return_found;

			return (lexeme){
			    .tag = lexeme::word,
			    .value.word = word,
			};
		}
		if (c == ':') {
			l->in_trailing = true;
			l->state = lex_state::in_word;
			return (lexeme){.tag = lexeme::nothing};
		}
		if (c != ' ' && c != '\r' && c != '\n') {
			l->word += c;
			return (lexeme){.tag = lexeme::nothing};
		} else {

			if (!l->word.empty()) {
				// Return the word as a lexeme (parameter)
				lexeme param = {.tag = lexeme::word,
						.value.word =
						    strdup(l->word.c_str())};
				l->word.clear();
				l->state = lex_state::out_of_word;
				return param;
			} else {
				l->state = lex_state::out_of_word;
				return (lexeme){.tag = lexeme::nothing};
			}
		}
		break;
	}
	assert(0);
}

std::vector<lexeme> lex_string(std::string string, lex_state *state) {
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

void print_message(message m) {
	std::cout << "prefix: " << (m.prefix.has_value ? m.prefix() : "") << std::endl
		  << "command: " << m.command << std::endl
		  << "params_count: " << m.params.size() << std::endl;
	for (size_t i = 0; i < m.params.size(); i++) {
		std::cout << "\t" << m.params[i] << std::endl;
	}
}

parseme parse(lexeme l, parse_state &p) {
	switch (l.tag) {
	case lexeme::carriage_return_line_feed: {
		if (p.words.size() == 0) {
			return (parseme){
			    .tag = parseme::error,
				.value = {
					.error = parse_error::no_command,
				},
			};
		}

		message m = {
			.prefix = p.prefix,
			.command = p.words[0],
			.params = p.words,
		};

		m.params.erase(m.params.begin());

		p = parse_state();

		return (parseme){
		    .tag = parseme::message,
		    .value = {
				.message = m,
			},
		};
		break;
	}
	case lexeme::word: {
		std::string word = l.value.word;

		if (word.size() == 0) throw std::runtime_error("IRC word's size can't be zero!");

		if (word[0] == ':' && p.words.size() == 0) {
			std::string without_colon = word.substr(1);
			p.prefix = optional<std::string>(without_colon);
		} else {
			p.words.push_back(word);
		}

		return (parseme){.tag = parseme::nothing};
	}
	default:
		return (parseme){.tag = parseme::error};
	}
}

std::vector<parseme> parse_lexeme_string(std::vector<lexeme> lexemes,
					 parse_state &state) {
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
