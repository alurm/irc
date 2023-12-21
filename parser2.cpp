
#include <iostream>
#include <vector>
#include <cstring>

struct message
{
    char **content;
    int length;
};

struct parse_result
{
    enum
    {
        word,
        nothing,
        message,
        // To-do!!!!!
        error,
    } tag;
    union
    {
        char *word;
        char nothing;
        struct message message;
    } content;
};

struct parse_state
{
    enum
    {
        space_or_start_of_word,
        letter_or_end_of_word,
        // To-do!!!!!
        carriage_return,
        line_feed,
    } state;
    std::string word;
    std::vector<std::string> message;
};

parse_result parse(char c, parse_state *p, const char *string, int i)
{
    switch (p->state)
    {
    case parse_state::space_or_start_of_word:
        if (c == '\r' && string[i + 1] == '\n')
        {
            p->state = parse_state::carriage_return;
            return (parse_result){.tag = parse_result::nothing};
        }
        else if (c != ' ')
        {
            p->word += c;
            p->state = parse_state::letter_or_end_of_word;
            return (parse_result){.tag = parse_result::nothing};
        }
        else
        {
            return (parse_result){.tag = parse_result::nothing};
        }
        break;

    case parse_state::letter_or_end_of_word:
        if (c == '\r' && string[i + 1] == '\n')
        {
            p->message.push_back(p->word);
            p->word = "";
            p->state = parse_state::carriage_return;
            return (parse_result){.tag = parse_result::nothing};
        }
        else if (c == ' ')
        {
            p->state = parse_state::space_or_start_of_word;
            p->message.push_back(p->word);
            p->word = "";
            return (parse_result){.tag = parse_result::nothing};
        }
        else
        {
            p->word += c;
            return (parse_result){.tag = parse_result::nothing};
        }
        break;

    case parse_state::carriage_return:
        if (c == '\n')
        {
            size_t length = p->message.size();
            char **content = new char *[length];
            for (int i = 0; i < length; i++)
            {
                content[i] = strdup(p->message[i].c_str());
            }
            p->message.clear();
            p->state = parse_state::space_or_start_of_word;
            return (parse_result){
                .tag = parse_result::message,
                .content.message = {
                    .length = static_cast<int>(length),
                    .content = content,
                },
            };
        }
        break;
    }
    return (parse_result){.tag = parse_result::nothing};
}

std::vector<parse_result> parse_string(const char *string, parse_state *state)
{
    char c;
    std::vector<parse_result> result;
    for (int i = 0; (c = string[i]) != 0; i++)
    {
        parse_result r = parse(c, state, string, i);
        if (r.tag != parse_result::nothing)
        {
            result.push_back(r);
        }
    }
    return result;
}

int main() {
    parse_state s = {
        .state = parse_state::space_or_start_of_word,
    };

    auto r = parse_string("?>>>>>\r\n\n\n\n\n\n\n<<<<<<######\r\nHow are you?\r\n", &s);

    for (size_t i = 0; i < r.size(); ++i) {
        if (r[i].tag == parse_result::message) {
            message m = r[i].content.message;
            for (int j = 0; j < m.length; j++) {
                std::cout << m.content[j] << std::endl;
            }
        }
    }

    return 0;
}