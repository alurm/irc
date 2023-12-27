#pragma once

#include <iostream>
#include <string>
#include <ctime>

class IRCResponse {
public:
    static std::string ERR_NOTREGISTERED(const std::string& source) {
        return "451 " + source + " :You have not registered";
    }
    static std::string ERR_ALREADYREGISTERED(const std::string& source) {
        return "462 " + source + " :You may not register";
    }
    static std::string ERR_PASSWDMISMATCH(const std::string& source) {
        return "464 " + source +  " :Password is incorrect";
    }
    static std::string ERR_NONICKNAMEGIVEN(const std::string& source) {
        return "431 " + source +  " :Nickname not given";
    }
    static std::string ERR_NICKNAMEINUSE(const std::string& source) {
        return "433 " + source + " " + source  + " :Nickname is already in use";
    }
    static std::string ERR_UNKNOWNCOMMAND(const std::string& source, const std::string& command) {
        return  "421 " + source + " " + command + " :Unknown command";
    }
    static std::string ERR_NEEDMOREPARAMS(const std::string& source, const std::string& command) {
        return  "461 " + source + " " + command + " :Not enough parameters";
    }
    static std::string ERR_TOOMANYCHANNELS(const std::string& source, const std::string& channel) {
        return  "405 " + source + " " + channel + " :You have joined too many channels";
    }
    static std::string ERR_NOTONCHANNEL(const std::string& source, const std::string& channel) {
        return  "442 " + source + " " + channel + " :You're not on that channel";
    }
    static std::string ERR_NOSUCHCHANNEL(const std::string& source, const std::string& channel) {
        return  "403 " + source + " " + channel + " :No such channel";
    }
    static std::string ERR_BADCHANNELKEY(const std::string& source, const std::string& channel) {
        return  "475 " + source + " " + channel + " :Cannot join channel (+k)";
    }
    static std::string ERR_CHANNELISFULL(const std::string& source, const std::string& channel) {
        return "471 " + source + " " + channel + " :Cannot join channel (+l)";
    }
    static std::string ERR_CANNOTSENDTOCHAN(const std::string& source, const std::string& channel) {
        return "404 " + source + " " + channel + " :Cannot send to channel";
    }
    static std::string ERR_CHANOPRIVSNEEDED(const std::string& source, const std::string& channel) {
        return "482 " + source + " " + channel + " :You're not channel operator";
    }
    static std::string ERR_CHANOPRIVSNEEDED(const std::string& source, const std::string& channel) {
        return "482 " + source + " " + channel + " :You're not channel operator";
    }
    static std::string ERR_NOSUCHNICK(const std::string& source, const std::string& nickname) {
        return "401 " + source + " " + nickname + " :No such nick/channel";
    }
    static std::string ERR_USERNOTINCHANNEL(const std::string& source, const std::string& nickname, const std::string& channel) {
        return "441 " + source + " " + nickname + " " + channel + " :They aren't on that channel";
    }
    /* Numeric Responses */
    static std::string RPL_WELCOME(const std::string& source) {
        return "001 " + source + " :Welcome " + source + " to the ft_irc network";
    }
    static std::string RPL_NAMREPLY(const std::string& source, const std::string& channel, const std::string& users) {
        return "353 " + source + " = " + channel + " :" + users;
    }
    static std::string RPL_ENDOFNAMES(const std::string& source, const std::string& channel) {
        return "366 " + source + " " + channel + " :End of /NAMES list.";
    }    
    /* Command Responses */   
    static std::string RPL_JOIN(const std::string& source, const std::string& channel) {
        return ":" + source + " JOIN :" + channel;
    }
    static std::string RPL_PART(const std::string& source, const std::string& channel) {
        return ":" + source + " PART :" + channel;
    }
    static std::string RPL_PING(const std::string& source, const std::string& token) {
        return ":" + source + " PONG :" + token;
    }
    static std::string RPL_PRIVMSG(const std::string& source, const std::string& target, const std::string& message) {
        return ":" + source + " PRIVMSG " + target + " :" + message;
    }
    static std::string RPL_NOTICE(const std::string& source, const std::string& target, const std::string& message) {
        return ":" + source + " NOTICE " + target + " :" + message;
    }
    static std::string RPL_QUIT(const std::string& source, const std::string& message) {
        return ":" + source + " QUIT :Quit: " + message;
    }
    static std::string RPL_KICK(const std::string& source, const std::string& channel, const std::string& target, const std::string& reason) {
        return ":" + source + " KICK " + channel + " " + target + " :" + reason;
    }
    static std::string RPL_MODE(const std::string& source, const std::string& channel, const std::string& modes, const std::string& args) {
        return ":" + source + " MODE " + channel + " " + modes + " " + args;
    }
};