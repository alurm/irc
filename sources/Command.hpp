#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Client.hpp"
#include "IRCResponse.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include <numeric>
#include <string>

struct Client;
struct Server;

class Command {
      protected:
	Server *server;
	bool is_auth;

      public:
	explicit Command(Server *s,  bool auth);
	virtual ~Command();
	bool isAuthenticationRequired() const;
	virtual void execute(Client &client, std::vector<std::string> args) = 0;
};

class Pass : public Command {
      public:
	Pass(Server *server, bool auth);
	~Pass();

	void execute(Client &client, std::vector<std::string> args);
};

class Join : public Command
{
    public:

        Join(Server* server,  bool auth);
        ~Join();

        void    execute(Client &client, std::vector<std::string> args);
};

class Nick : public Command
{
    public:

        Nick(Server* server,  bool auth);
        ~Nick();

        void    execute(Client &client, std::vector<std::string> args);
};


class User : public Command
{
    public:

        User(Server* server,  bool auth);
        ~User();

        void    execute(Client &client, std::vector<std::string> args);
};

class Quit : public Command
{
    public:

        Quit(Server* server,  bool auth);
        ~Quit();

        void    execute(Client &client, std::vector<std::string> args);
};


class Mode : public Command
{
    public:

        Mode(Server* server,  bool auth);
        ~Mode();

        void    execute(Client &client, std::vector<std::string> args);
};

class Topic : public Command
{
    public:

        Topic(Server* server,  bool auth);
        ~Topic();

        void    execute(Client &client, std::vector<std::string> args);
};

class Ping : public Command
{
    public:

        Ping(Server* server,  bool auth);
        ~Ping();

        void    execute(Client &client, std::vector<std::string> args);
};

class PrivMsg : public Command
{
    public:

        PrivMsg(Server* server,  bool auth);
        ~PrivMsg();

        void    execute(Client &client, std::vector<std::string> args);
};

class Pong : public Command
{
    public:

        Pong(Server* server,  bool auth);
        ~Pong();

        void    execute(Client &client, std::vector<std::string> args);
};

class Kick : public Command
{
    public:

        Kick(Server* server,  bool auth);
        ~Kick();

        void    execute(Client &client, std::vector<std::string> args);
};

class Invite : public Command
{
    public:

        Invite(Server* server,  bool auth);
        ~Invite();

        void    execute(Client &client, std::vector<std::string> args);
};

class Part : public Command
{
    public:

        Part(Server* server,  bool auth);
        ~Part();

        void    execute(Client &client, std::vector<std::string> args);
};

class Who : public Command
{
    public:

        Who(Server* server,  bool auth);
        ~Who();

        void    execute(Client &client, std::vector<std::string> args);
};

class Cap : public Command
{
    public:

        Cap(Server* server,  bool auth);
        ~Cap();

        void    execute(Client &client, std::vector<std::string> args);
};

#endif