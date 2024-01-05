#pragma once

#include "Server.hpp"
#include "Client.hpp"

class Client;
class Server;

class Channel 
{
    typedef std::vector<Client *>::iterator client_iterator;

    private:

        std::string             name;
        Client*                 admin;
        std::vector<Client *>   clients;
		std::string				key;
        std::vector<Client *>   limits;
        bool                    message;


        Channel();
        Channel(const Channel& src);
    public:
        void sendAll(const std::string &message);
        std::string getName() const;
        void add_client(Client *client) { clients.push_back(client); }
        std::vector<std::string> getNicknames();
        Channel(const std::string &name, const std::string &key, Client* admin);
        ~Channel();
                void                        handleClientRemoval(Client* client);

};
