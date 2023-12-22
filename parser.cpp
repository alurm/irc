#include <stddef.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <assert.h>
#include <vector>

struct message {
	char **content;
	int length;
};

struct parse_result {
	enum {
		word,
		nothing,
		message,
		// To-do!!!!!
		error,
	} tag;
	union {
		char *word;
		char nothing;
		struct message message;
	} content;
};

struct parse_state {
	enum {
		space_or_start_of_word,
		letter_or_end_of_word,
		// To-do!!!!!
		// carriage_return,
		// line_feed,
	} state;
	std::string word;
	std::vector<std::string> message;
};

struct lexeme {
	enum lexeme_tag {
		spaces,
		carriage_return_line_feed,
		error,
		word,
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
	// switch (p->state) {
	// case parse_state::space_or_start_of_word:
	// 	if (c == '\n') {
	// 		size_t length = p->message.size();
	// 		char **content = (char **)malloc(sizeof(char *) * length);
	// 		for (int i = 0; i < length; i++) {
	// 			content[i] = strdup(p->message[i].c_str());
	// 		}
	// 		p->state = parse_state::space_or_start_of_word;
	// 		p->message.erase(p->message.begin(), p->message.end());
	// 		return (parse_result){
	// 			.tag = parse_result::message,
	// 			.content.message = {
	// 				.length = length,
	// 				.content = content,
	// 			},
	// 		};
	// 	}
	// 	else if (c != ' ') {
	// 		p->word += c;
	// 		p->state = parse_state::letter_or_end_of_word;
	// 		return (parse_result){ .tag = parse_result::nothing };
	// 	} else {
	// 		return (parse_result){ .tag = parse_result::nothing };
	// 	}
	// break;

	// case parse_state::letter_or_end_of_word:
	// 	if (c == '\n') {
	// 		p->message.push_back(p->word);
	// 		p->word = "";

	// 		size_t length = p->message.size();
	// 		char **content = (char **)malloc(sizeof(char *) * length);
	// 		for (int i = 0; i < length; i++) {
	// 			content[i] = strdup(p->message[i].c_str());
	// 		}
	// 		p->state = parse_state::space_or_start_of_word;
	// 		p->message.erase(p->message.begin(), p->message.end());
	// 		return (parse_result){
	// 			.tag = parse_result::message,
	// 			.content.message = {
	// 				.length = length,
	// 				.content = content,
	// 			},
	// 		};		
	// 	}
	// 	else if (c == ' ') {
	// 		p->state = parse_state::space_or_start_of_word;
	// 		p->message.push_back(p->word);
	// 		p->word = "";
	// 		return (parse_result){ .tag = parse_result::nothing, };
	// 	} else {
	// 		p->word += c;
	// 		return (parse_result){ .tag = parse_result::nothing };
	// 	}
	// break;
	// }
}

parse_result parse(char c, parse_state *p) {
	switch (p->state) {
	case parse_state::space_or_start_of_word:
		if (c == '\n') {
			size_t length = p->message.size();
			char **content = (char **)malloc(sizeof(char *) * length);
			for (int i = 0; i < length; i++) {
				content[i] = strdup(p->message[i].c_str());
			}
			p->state = parse_state::space_or_start_of_word;
			p->message.erase(p->message.begin(), p->message.end());
			return (parse_result){
				.tag = parse_result::message,
				.content.message = {
					.length = length,
					.content = content,
				},
			};
		}
		else if (c != ' ') {
			p->word += c;
			p->state = parse_state::letter_or_end_of_word;
			return (parse_result){ .tag = parse_result::nothing };
		} else {
			return (parse_result){ .tag = parse_result::nothing };
		}
	break;

	case parse_state::letter_or_end_of_word:
		if (c == '\n') {
			p->message.push_back(p->word);
			p->word = "";

			size_t length = p->message.size();
			char **content = (char **)malloc(sizeof(char *) * length);
			for (int i = 0; i < length; i++) {
				content[i] = strdup(p->message[i].c_str());
			}
			p->state = parse_state::space_or_start_of_word;
			p->message.erase(p->message.begin(), p->message.end());
			return (parse_result){
				.tag = parse_result::message,
				.content.message = {
					.length = length,
					.content = content,
				},
			};		
		}
		else if (c == ' ') {
			p->state = parse_state::space_or_start_of_word;
			p->message.push_back(p->word);
			p->word = "";
			return (parse_result){ .tag = parse_result::nothing, };
		} else {
			p->word += c;
			return (parse_result){ .tag = parse_result::nothing };
		}
	break;
	}
}

std::vector<parse_result> parse_string(
	const char *string,
	parse_state *state
) {
	char c;
	std::vector<parse_result> result;
	for (int i = 0; (c = string[i]) != 0; i++) {
		parse_result r = parse(c, state);
		if (r.tag != parse_result::nothing) {
			result.push_back(r);
		}
	}
	return result;
}

int main() {
	// {
	// 	parse_state s = {
	// 		.state = parse_state::space_or_start_of_word,
	// 	};

	// 	{
	// 		std::vector<parse_result> r = parse_string("char ", &s);
	// 		parse_result last = r[r.size() - 1];
	// 		assert(last.tag == parse_result::word);
	// 	}
	// 	{
	// 		std::vector<parse_result> r = parse_string("doge ", &s);
	// 		parse_result last = r[r.size() - 1];
	// 		assert(last.tag == parse_result::word);
	// 	}
	// }
	{
		parse_state s = {
			.state = parse_state::space_or_start_of_word,
		};

		{
			parse_string("char ", &s);
			std::vector<parse_result> r = parse_string("doge\n", &s);
			assert(r.size() == 1);
			parse_result last = r[r.size() - 1];
			assert(last.tag == parse_result::message);
			message m = last.content.message;
			for (int i = 0; i < m.length; i++) {
				printf("%s\n", m.content[i]);
			}
		}
		// {
		// 	std::vector<parse_result> r = parse_string("doge ", &s);
		// 	parse_result last = r[r.size() - 1];
		// 	assert(last.tag == parse_result::word);
		// }
	}
}