#include <stddef.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <assert.h>

struct parse_result {
	enum parse_result_tag {
		word,
		nothing,
	} tag;
	union {
		char *word;
		char nothing;
	} content;
};

parse_result parse(char c) {
	static void *state = &&space_or_start_of_word;
	static std::string word;

	goto *state;

space_or_start_of_word:

	if (c != ' ') {
		word += c;
		state = &&letter_or_end_of_word;
	}

	return (parse_result){ .tag = parse_result::nothing };

letter_or_end_of_word:

	if (c == ' ') {
		state = &&space_or_start_of_word;
		char *word_pointer = strdup(word.c_str());
		word = "";
		return (parse_result){
			.tag = parse_result::word,
			.content.word = word_pointer,
		};
	} else {
		word += c;
		return (parse_result){ .tag = parse_result::nothing };
	}
}

int main() {
	parse('c');
	parse('h');
	parse('a');
	parse('r');
	{
		parse_result pr = parse(' ');
		assert(pr.tag == parse_result::word);
		printf("%s\n", pr.content.word);
	}
	parse('d');
	parse('o');
	parse('g');
	parse('e');
	{
		parse_result pr = parse(' ');
		assert(pr.tag == parse_result::word);
		printf("%s\n", pr.content.word);
	}
}
