#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &key, Client *admin)
    : name(name), admin(admin), key(key), limit(0), message(false) {}

std::string Channel::getName() const { return name; }

std::vector<std::string> Channel::getNicknames() {
    std::vector<std::string> nicknames;

    for (client_iterator it = clients.begin(); it != clients.end(); ++it) {
        Client *client = *it;

        std::string nick = (client == admin ? "@" : "") + client->getNickname();
        nicknames.push_back(nick);
    }

    return nicknames;
}


void    Channel::sendAll(const std::string &message) {
    for (client_iterator it = clients.begin(); it != clients.end(); ++it) {
        (*it)->sendWithLineEnding(message);
    }
}

// void    Channel::broadcast(const std::string& message, Client* exclude)
// {
//     client_iterator it_b = clients.begin();
//     client_iterator it_e = clients.end();

//     while (it_b != it_e)
//     {
//         if (*it_b == exclude)
//         {
//             it_b++;
//             continue;
//         }

//         (*it_b)->sendWithLineEnding(message);
//         it_b++;
//     }
// }

std::string Channel::getKey() const { return key; }

size_t Channel::getLimit() const {
    return limit;
}

size_t  Channel::getClientCount()const { return clients.size(); }


void Channel::handleClientRemoval(Client *client) {
    client_iterator it = clients.begin();

    while (it != clients.end()) {
        if (*it == client) {
            it = clients.erase(it);
        } else {
            ++it;
        }
    }

    client->setChannel(NULL);

    if (client == admin) {
        if (!clients.empty()) {
            admin = *(clients.begin());

            std::string message = client->getNickname() +
                " is now the admin of the channel " +
                name;
            std::cout << message << std::endl;
        } else {
            admin = NULL; 
        }
    }
}

Channel::~Channel() {}