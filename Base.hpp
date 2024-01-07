#ifndef Base_HPP
#define Base_HPP

#include <numeric>
#include <string>

class Base;

#include "Server.hpp"
#include "Client.hpp"
#include "Parser.hpp"

class Server;
class Client;

class Base
{
    protected:

        Server* _srv;
        bool    _auth;

        Base();
        Base(const Base& src);

    public:

        explicit Base(Server* srv, bool auth = true);
        virtual ~Base();

        bool auth_required() const;

        virtual void execute(Client* client, std::vector<std::string> args) = 0;
        void dispatch(Server *s, Client *c, message m);
};

// class Notice : public Base
// {
//     public:

//         Notice(Server* srv);
//         ~Notice();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class PrivMsg : public Base
// {
//     public:

//         PrivMsg(Server* srv);
//         ~PrivMsg();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class Part : public Base
// {
//     public:

//         Part(Server* srv);
//         ~Part();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class Quit : public Base
// {
//     public:

//         Quit(Server* srv, bool auth);
//         ~Quit();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class Join : public Base
// {
//     public:

//         Join(Server* srv);
//         ~Join();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class User : public Base
// {
//     public:

//         User(Server* srvi, bool auth);
//         ~User();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class Nick : public Base
// {
//     public:

//         Nick(Server* srv, bool auth);
//         ~Nick();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class Pass : public Base
// {
//     public:

//         Pass(Server* srv, bool auth);
//         ~Pass();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class Kick : public Base
// {
//     public:

//         Kick(Server* srv);
//         ~Kick();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class Ping : public Base
// {
//     public:

//         Ping(Server* srv);
//         ~Ping();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class Pong : public Base
// {
//     public:

//         Pong(Server* srv);
//         ~Pong();

//         void    execute(Client* client, std::vector<std::string> args);
// };

// class Mode : public Base
// {
//     public:

//         Mode(Server* srv);
//         ~Mode();

//         void    execute(Client* client, std::vector<std::string> args);
// };

#endif