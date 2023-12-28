#pragma once

#include "Server.hpp"
#include "Client.hpp"

class Channel 
{
    typedef std::vector<Client *>::iterator client_iterator;

    private:

        std::string             name;
		std::string				key;
        Client*                 admin;
        std::vector<Client *>   clients;

        Channel();
        Channel(const Channel& src);
    
    public:

        Channel(const std::string &name, const std::string &key, Client* admin);
        ~Channel();
};
