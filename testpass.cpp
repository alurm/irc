#include "Parser.hpp"
#include <iostream>
#include <cassert>

int main() {
    // Lex.

    lex_state state = {
        .state = lex_state::in_word,
        .word = "",
        .in_trailing = false,
    };

    // Test case: "PASS \r\n"
    std::vector<lexeme> lexemes = lex_string("PASS \r\n", &state);

    // Ensure correct lexing.
    assert(lexemes.size() == 3);
    assert(lexemes[0].tag == lexeme::word && strcmp(lexemes[0].value.word, "PASS") == 0);
    assert(lexemes[1].tag == lexeme::word);
    assert(lexemes[2].tag == lexeme::carriage_return_line_feed);
    // Parse.
    parse_state parse_state = {
        .prefix = {
            .has_value = false,
        },
    };
    std::vector<parseme> parsemes = parse_lexeme_string(lexemes, &parse_state);

    // Ensure correct parsing.
    assert(parsemes.size() == 1 && parsemes[0].tag == parseme::message);
    message m = parsemes[0].value.message;
    assert(strcmp(m.command, "PASS") == 0);
    
    std::cout << "Test passed!\n";

    return 0;
}
