#ifndef PASS_HPP
#define PASS_HPP

#include "Client.hpp"
#include "IRCResponse.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include <numeric>
#include <string>

class Server;
class Client;

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
	virtual void execute(Client *client, std::vector<std::string> args) = 0;
};

class Pass : public Base2 {
      public:
	Pass(Server *srv, bool auth);
	~Pass();

	void execute(Client *client, std::vector<std::string> args);
};

class Join : public Base2
{
    public:

        Join(Server* srv,  bool auth);
        ~Join();

        void    execute(Client* client, std::vector<std::string> args);
};

class Nick : public Base2
{
    public:

        Nick(Server* srv,  bool auth);
        ~Nick();

        void    execute(Client* client, std::vector<std::string> args);
};


class User : public Base2
{
    public:

        User(Server* server,  bool auth);
        ~User();

        void    execute(Client* client, std::vector<std::string> args);
};


#endif
