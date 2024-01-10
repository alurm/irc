// #include "dispatch.cpp"
#include "Parser.hpp"

int main() {
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