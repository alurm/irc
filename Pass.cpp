#include "Pass.hpp"
Base2::Base2(Server *srv, bool auth) : _srv(srv), _auth(auth) {}

Base2::~Base2() {}

bool Base2::auth_required() const { return _auth; }

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

    if (_srv->getPassword() != args[0].substr(args[0][0] == ':' ? 1 : 0))
    {
        client->respondWithPrefix(IRCResponse::ERR_PASSWDMISMATCH(client->getNickname()));
        return;
    }

    client->setState(client_state::LOGIN);
}
