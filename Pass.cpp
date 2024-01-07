#include "Pass.hpp"

Pass::Pass(Server* srv, bool auth) : Base(srv, auth) {}

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

    if (_srv->getPassword() != args[0].substr(args[0][0] == ':' ? 1 : 0))
    {
        client->respondWithPrefix(IRCResponse::ERR_PASSWDMISMATCH(client->getNickname()));
        return;
    }

    client->setState(client_state::LOGIN);
}
