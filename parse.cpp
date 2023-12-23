#include <stddef.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <assert.h>
#include <vector>

// Lexer.

struct lexeme {
	enum lexeme_tag {
		carriage_return_line_feed,
		word,

		error,
		nothing,

		// Errors. To-do: find a better way.
		error_carriage_return_or_line_feed,
		error_nil,
	} tag;
	union {
		char *word;
		lexeme_tag error;
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

lexeme lex(char c, lex_state *l) {
	if (c == 0) return (lexeme){
		.tag = lexeme::error,
		.value.error = lexeme::error_nil,
	};
	if (c == '\n' && l->state != lex_state::carriage_return_found) {
		return (lexeme){
			.tag = lexeme::error,
			.value.error = lexeme::error_carriage_return_or_line_feed,
		};
	}

	switch (l->state) {
	case lex_state::carriage_return_found:
		return (lexeme){ .tag = lexeme::carriage_return_line_feed };
	break;
	case lex_state::in_word:
		if (l->in_trailing) {
			if (c == '\r') {
				l->state = lex_state::carriage_return_found;
				return (lexeme){
					.tag = lexeme::word,
					.value.word = strdup(l->word.c_str()),
				};
			}
			l->word += c;
			return (lexeme){ .tag = lexeme::nothing };
		}
		if (c == '\r') {
			return (lexeme){
				.tag = lexeme::error,
				.value.error = lexeme::error_carriage_return_or_line_feed,
			};
		}
		if (c == ' ') {
			l->state = lex_state::out_of_word;
			char *word = strdup(l->word.c_str());
			l->word = "";
			return (lexeme){
				.tag = lexeme::word,
				.value.word = word,
			};
		}
		l->word += c;
		return (lexeme){ .tag = lexeme::nothing };
	break;
	case lex_state::out_of_word:
		if (c == ':') {
			l->in_trailing = true;
			l->state = lex_state::in_word;
			return (lexeme){ .tag = lexeme::nothing };
		}
		if (c == ' ') {
			return (lexeme){ .tag = lexeme::nothing };
		}
		if (c == '\r') {
			l->state = lex_state::carriage_return_found;
			return (lexeme){ .tag = lexeme::nothing };
		}
		l->state = lex_state::in_word;
		l->word += c;
		return (lexeme){ .tag = lexeme::nothing };
	break;
	}
	assert(0);
}

std::vector<lexeme> lex_string(
	const char *string,
	lex_state *state
) {
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

struct message {
	char *prefix;
	bool is_prefixed;
	char *command;
	char **params;
	int params_count;
};

struct parseme {
	enum parseme_tag {
		nothing,
		message,
		error,

		// Errors. To-do: find a better way.
		// (To-do.)
	} tag;
	union {
		::message message;
		parseme_tag error;
	} value;
};

struct parse_state;

parseme parse(lexeme l, parse_state *p);
