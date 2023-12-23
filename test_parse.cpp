#include "parse.cpp"

int main() {
    lex_state state = {
		.state = lex_state::in_word,
		.word = "",
		.in_trailing = false,
	};

    std::vector<lexeme> lexemes = lex_string(
        ":Nickname!username@hostname.com PRIVMSG #channel :Hello everyone! How are you today?\r\n",
        &state
    );

    {
        lexeme l = lexemes[0];
        assert(l.tag == lexeme::word && strcmp(l.value.word, ":Nickname!username@hostname.com") == 0);
    }

    {
        lexeme l = lexemes[1];
        assert(l.tag == lexeme::word && strcmp(l.value.word, "PRIVMSG") == 0);
    }

    {
        lexeme l = lexemes[2];
        assert(l.tag == lexeme::word && strcmp(l.value.word, "#channel") == 0);
    }

    {
        lexeme l = lexemes[3];
        assert(l.tag == lexeme::word && strcmp(l.value.word, "Hello everyone! How are you today?") == 0);
    }

    {
        lexeme l = lexemes[4];
        assert(l.tag == lexeme::carriage_return_line_feed);
    }

    assert(lexemes.size() == 5);

    printf("ok\n");
}
