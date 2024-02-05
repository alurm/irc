#include "Parser.hpp"

void pass_test() {
	{
		// Lex.

		lex_state state;

		std::vector<lexeme> lexemes = lex_string("PASS ::\r\n", state);
		{
			assert(lexemes.size() == 3);
			{
				lexeme l = lexemes[0];
				assert(l.tag == lexeme::word &&
					l.value.word == "PASS");

				{
					lexeme l = lexemes[1];
					assert(l.tag == lexeme::word);
					std::cout << "'" << l.value.word << "'" << std::endl;
					assert(l.value.word == ":");
				}

				{
					lexeme l = lexemes[2];
					assert(l.tag == lexeme::terminator);
				}

				printf("lex test: ok\n");
			}

			// Parse.

			{
				parse_state p;

				std::vector<message> parsemes =
					parse_lexeme_string(lexemes, p);

				std::cout << "parsemes.size is " << parsemes.size()
					<< std::endl;

				assert(parsemes.size() == 1);

				message m = parsemes[0];

				assert(m.command == "PASS");
				assert(m.params.size() == 1);
				assert(m.params[0] == ":");

				printf("parse test: ok\n");
			}
		}
	}

	{
		lex_state state;

		std::vector<lexeme> lexemes = lex_string("PASS\r\n", state);

		assert(lexemes.size() == 2);
		// assert(lexemes[0].tag == lexeme::error && lexemes[0].value.error == lex_error::carriage_return_or_line_feed);
		// // assert(lexemes[1].tag == lexeme::carriage_return_line_feed);

		// parse_state parse_state = {
		// 	.prefix = {
		// 		.has_value = false,
		// 	},
		// };
		// std::vector<parseme> parsemes = parse_lexeme_string(lexemes, &parse_state);

		// assert(parsemes.size() == 1 && parsemes[0].tag == parseme::message);
		// message m = parsemes[0].value.message;
		// assert(strcmp(m.command, "PASS") == 0);

		std::cout << "Test passed!\n";
	}

	{
		// Lex.

		lex_state state;

		// Test case: "PASS \r\n"
		std::vector<lexeme> lexemes = lex_string("PASS \r\n", state);

		// Ensure correct lexing.
		assert(lexemes.size() == 2);
		assert(lexemes[0].tag == lexeme::word && lexemes[0].value.word == "PASS");
		assert(lexemes[1].tag == lexeme::terminator);

		// Parse.
		parse_state parse_state;

		std::vector<message> parsemes = parse_lexeme_string(lexemes, parse_state);

		// Ensure correct parsing.
		assert(parsemes.size() == 1);
		message m = parsemes[0];
		assert(m.command == "PASS");

		std::cout << "Test passed!\n";
	}
}

void print_lexemes(std::vector<lexeme> lexemes) {
	for (std::vector<lexeme>::iterator it = lexemes.begin(); it != lexemes.end(); it++) {
		if ((*it).tag == lexeme::word) {
			std::cout << (*it).value.word << std::endl;
		} else {
			std::cout << "terminator" << std::endl;
		};
	}
}

void generic_parser_test() {
	// Lex.

	lex_state state;

	std::vector<lexeme> lexemes =
	    lex_string(":Nickname!username@hostname.com PRIVMSG #channel :Hello everyone! How are you today?\r\n",
		       state);
	
	std::cout << lexemes.size() << std::endl;
	
	assert(lexemes.size() == 5);

	{
		{
			lexeme l = lexemes[0];
			assert(l.tag == lexeme::word &&
			       l.value.word == ":Nickname!username@hostname.com");
		}

		{
			lexeme l = lexemes[1];
			assert(l.tag == lexeme::word &&
			       l.value.word == "PRIVMSG");
		}

		{
			lexeme l = lexemes[2];
			assert(l.tag == lexeme::word &&
			       l.value.word == "#channel");
		}

		{
			lexeme l = lexemes[3];
			assert(l.tag == lexeme::word &&
			       l.value.word == "Hello everyone! How are you today?");
		}

		{
			lexeme l = lexemes[4];
			assert(l.tag == lexeme::terminator);
		}

		printf("lex test: ok\n");
	}

	// Parse.

	{
		parse_state p;

		std::vector<message> parsemes =
		    parse_lexeme_string(lexemes, p);

		assert(parsemes.size() == 1);

		message m = parsemes[0];

		assert(m.prefix.has_value && m.prefix() == "Nickname!username@hostname.com");
		assert(m.command == "PRIVMSG");
		assert(m.params.size() == 2);
		assert(m.params[0] == "#channel");
		assert(m.params[1] ==
			      "Hello everyone! How are you today?");
		printf("parse test: ok\n");
	}

	// // Dispatch.

	// {
	// server s = {
	// .password = const_cast<char *>("234"),
	// };

	// {
	// client c = { .state = client::unregistered };

	// assert(dispatch(
	// &s,
	// &c,
	// (message){
	// .command = const_cast<char *>("PASS"),
	// .params = const_cast<char **>((const char *[]){
	// "123",
	// }),
	// .params_count = 1,
	// }
	// ).has_value == false);
	// }

	// {
	// client c = { .state = client::registered };

	// optional<message> om = dispatch(
	// &s,
	// &c,
	// (message){
	// .command = const_cast<char *>("PASS"),
	// .params = const_cast<char **>((const char *[]){
	// "123",
	// }),
	// .params_count = 1,
	// }
	// );

	// assert(om.has_value);

	// assert(strcmp(om.value.command, ERR_ALREADYREGISTRED) == 0);
	// }

	// printf("dispatch test: ok\n");
	// }
}

#define STRING_COMMA_SYMBOL(SYMBOL) #SYMBOL, SYMBOL

int main() {
	struct test {
		const char *name;
		void (*test)();
		bool run;
	} tests[] = {
		{ STRING_COMMA_SYMBOL(pass_test), true },
		{ STRING_COMMA_SYMBOL(generic_parser_test), true },
	};

	for (unsigned long i = 0; i < sizeof(tests) / sizeof(*tests); i++) {
		struct test t = tests[i];
		std::cout << t.name << std::endl;
		if (t.run) {

			t.test();
		}
	}
}