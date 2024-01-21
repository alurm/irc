#include "dispatch.cpp"
#include "Parser.hpp"

void pass_test() {
	{
		// Lex.

		lex_state state = {
			.state = lex_state::in_word,
			.word = "",
			.in_trailing = false,
		};

		std::vector<lexeme> lexemes = lex_string("PASS ::\r\n", &state);
		{
			{
				lexeme l = lexemes[0];
				assert(l.tag == lexeme::word &&
					strcmp(l.value.word, "PASS") == 0);

				{
					lexeme l = lexemes[1];
					assert(l.tag == lexeme::word &&
						strcmp(l.value.word, ":") == 0);
				}

				{
									
					lexeme l = lexemes[2];
									std::cout << "l.tag is " << l.tag << std::endl;
					assert(l.tag == lexeme::carriage_return_line_feed);
				}

				assert(lexemes.size() == 3);

				printf("lex test: ok\n");
			}

			// Parse.

			{
				parse_state p = {
					.prefix =
					{
						.has_value = false,
					},
				};

				std::vector<parseme> parsemes =
					parse_lexeme_string(lexemes, &p);
				std::cout << "parsemes.size is " << parsemes.size()
					<< std::endl;
				std::cout << "parsemes[0].tag is " << parsemes[0].tag
					<< std::endl;

				assert(parsemes.size() == 1 &&
					parsemes[0].tag == parseme::message);

				message m = parsemes[0].value.message;

				assert(strcmp(m.command, "PASS") == 0);
				assert(m.params_count == 1);
				assert(strcmp(m.params[0], ":") == 0);
				printf("parse test: ok\n");
			}
		}
	}

	{
		lex_state state = {
				.state = lex_state::in_word,
				.word = "",
				.in_trailing = false,
			};

			std::vector<lexeme> lexemes = lex_string("PASS\r\n", &state);

			assert(lexemes.size() == 2);
			assert(lexemes[0].tag == lexeme::word && strcmp(lexemes[0].value.word, "PASS") == 0);
			assert(lexemes[1].tag == lexeme::carriage_return_line_feed);

			parse_state parse_state = {
				.prefix = {
					.has_value = false,
				},
			};
			std::vector<parseme> parsemes = parse_lexeme_string(lexemes, &parse_state);

			assert(parsemes.size() == 1 && parsemes[0].tag == parseme::message);
			message m = parsemes[0].value.message;
			assert(strcmp(m.command, "PASS") == 0);

			std::cout << "Test passed!\n";
	}
}

void generic_parser_test() {
	// Lex.

	lex_state state = {
	    .state = lex_state::in_word,
	    .word = "",
	    .in_trailing = false,
	};

	std::vector<lexeme> lexemes =
	    lex_string(":Nickname!username@hostname.com PRIVMSG #channel :Hello everyone! How are you today?\r\n",
		       &state);
	{
		{
			lexeme l = lexemes[0];
			assert(l.tag == lexeme::word &&
			       strcmp(l.value.word,
				      ":Nickname!username@hostname.com") == 0);
		}

		{
			lexeme l = lexemes[1];
			assert(l.tag == lexeme::word &&
			       strcmp(l.value.word, "PRIVMSG") == 0);
		}

		{
			lexeme l = lexemes[2];
			assert(l.tag == lexeme::word &&
			       strcmp(l.value.word, "#channel") == 0);
		}

		{
			lexeme l = lexemes[3];
			assert(l.tag == lexeme::word &&
			       strcmp(l.value.word,
				      "Hello everyone! How are you today?") ==
				   0);
		}

		{
			lexeme l = lexemes[4];
			assert(l.tag == lexeme::carriage_return_line_feed);
		}

		assert(lexemes.size() == 5);

		printf("lex test: ok\n");
	}

	// Parse.

	{
		parse_state p = {
		    .prefix =
			{
			    .has_value = false,
			},
		};

		std::vector<parseme> parsemes =
		    parse_lexeme_string(lexemes, &p);

		assert(parsemes.size() == 1 &&
		       parsemes[0].tag == parseme::message);

		message m = parsemes[0].value.message;

		assert(strcmp(m.prefix, "Nickname!username@hostname.com") == 0);
		assert(strcmp(m.command, "PRIVMSG") == 0);
		assert(m.params_count == 2);
		assert(strcmp(m.params[0], "#channel") == 0);
		assert(strcmp(m.params[1],
			      "Hello everyone! How are you today?") == 0);
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