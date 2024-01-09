# ifndef PASS_HPP
# define PASS_HPP

// #include "IRCResponse.hpp" 
#include <numeric>
#include <string>
#include "Server.hpp"
#include "Client.hpp"
#include "Parser.hpp"

class Server;
class Client;

class Base2 {
    protected:

        Server* _srv;
        bool    _auth;

        // Base2();
        // Base2(const Base2& src);

    public:

        explicit Base2(Server* srv, bool auth = true);
        virtual ~Base2();
        bool auth_required() const;
        virtual void execute(Client* client, std::vector<std::string> args) = 0;
};

class Pass : public Base2 {
public:

    Pass(Server* srv, bool auth);
    ~Pass();

void    execute(Client* client, std::vector<std::string> args);
};

#endif
