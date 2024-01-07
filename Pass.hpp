#ifndef PASS_HPP
#define PASS_HPP

#include "Base.hpp"
#include "IRCResponse.hpp"

class Pass : public Base
{
public:

    Pass(Server* srv, bool auth);
    ~Pass();

    void    execute(Client* client, std::vector<std::string> args);
};

#endif
