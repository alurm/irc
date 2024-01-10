#include "Pass.hpp"

Base2::Base2(Server *s, bool auth) : server(s), is_auth(auth) {
}

Base2::~Base2() {}

bool Base2::isAuthenticationRequired() const { return is_auth; }

Pass::Pass(Server* srv, bool auth) : Base2(srv, auth) {}

Pass::~Pass() {}

void    Pass::execute(Client* client, std::vector<std::string> args)
{
    std::cout << "in execute of pass\n";
    if (args.empty())
    {
        client->respondWithPrefix(IRCResponse::ERR_NEEDMOREPARAMS(client->getNickname(), "PASS"));
        return;
    }

    if (client->isInRegisteredState())
    {
        client->respondWithPrefix(IRCResponse::ERR_ALREADYREGISTERED(client->getNickname()));
        return;
    }

    if (server->getPassword() != args[0])
    {
        client->respondWithPrefix(IRCResponse::ERR_PASSWDMISMATCH(client->getNickname()));
        return;
    }

    client->setState(client_state::LOGIN);
}
