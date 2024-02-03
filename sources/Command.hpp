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

// Buggy?
class Base2 {
      protected:
	Server *server;
	bool is_auth;

	// Base2();
	// Base2(const Base2& src);

      public:
	explicit Base2(Server *s,  bool auth);
	virtual ~Base2();
	bool isAuthenticationRequired() const;
	virtual void execute(Client &client, std::vector<std::string> args) = 0;
};

class Pass : public Base2 {
      public:
	Pass(Server *server, bool auth);
	~Pass();

	void execute(Client &client, std::vector<std::string> args);
};

class Join : public Base2
{
    public:

        Join(Server* server,  bool auth);
        ~Join();

        void    execute(Client &client, std::vector<std::string> args);
};

class Nick : public Base2
{
    public:

        Nick(Server* server,  bool auth);
        ~Nick();

        void    execute(Client &client, std::vector<std::string> args);
};


class User : public Base2
{
    public:

        User(Server* server,  bool auth);
        ~User();

        void    execute(Client &client, std::vector<std::string> args);
};

class Quit : public Base2
{
    public:

        Quit(Server* server,  bool auth);
        ~Quit();

        void    execute(Client &client, std::vector<std::string> args);
};


class Mode : public Base2
{
    public:

        Mode(Server* server,  bool auth);
        ~Mode();

        void    execute(Client &client, std::vector<std::string> args);
};

class Topic : public Base2
{
    public:

        Topic(Server* server,  bool auth);
        ~Topic();

        void    execute(Client &client, std::vector<std::string> args);
};

class Ping : public Base2
{
    public:

        Ping(Server* server,  bool auth);
        ~Ping();

        void    execute(Client &client, std::vector<std::string> args);
};

class PrivMsg : public Base2
{
    public:

        PrivMsg(Server* server,  bool auth);
        ~PrivMsg();

        void    execute(Client &client, std::vector<std::string> args);
};

class Pong : public Base2
{
    public:

        Pong(Server* server,  bool auth);
        ~Pong();

        void    execute(Client &client, std::vector<std::string> args);
};

class Kick : public Base2
{
    public:

        Kick(Server* server,  bool auth);
        ~Kick();

        void    execute(Client &client, std::vector<std::string> args);
};

class Invite : public Base2
{
    public:

        Invite(Server* server,  bool auth);
        ~Invite();

        void    execute(Client &client, std::vector<std::string> args);
};

class Part : public Base2
{
    public:

        Part(Server* server,  bool auth);
        ~Part();

        void    execute(Client &client, std::vector<std::string> args);
};

class Who : public Base2
{
    public:

        Who(Server* server,  bool auth);
        ~Who();

        void    execute(Client &client, std::vector<std::string> args);
};

class Cap : public Base2
{
    public:

        Cap(Server* server,  bool auth);
        ~Cap();

        void    execute(Client &client, std::vector<std::string> args);
};

#endif