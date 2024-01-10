#include "dispatch.cpp"
#include "Parser.hpp"

int main() {
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
