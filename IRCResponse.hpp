#pragma once

#include <iostream>
#include <string>
#include <ctime>

class IRCResponse {
public:
    static std::string ERR_NOTREGISTERED(const std::string& str) {
        return "451 " + str + " :You have not registered";
    }
    static std::string ERR_ALREADYREGISTERED(const std::string& str) {
        return "462 " + str + " :You may not register";
    }
    static std::string ERR_PASSWDMISMATCH(const std::string& str) {
        return "464 " + str +  " :Password is incorrect";
    }
    static std::string ERR_NONICKNAMEGIVEN(const std::string& str) {
        return "431 " + str +  " :Nickname not given";
    }
    static std::string ERR_NICKNAMEINUSE(const std::string& str) {
        return "433 " + str + " " + str  + " :Nickname is already in use";
    }
    static std::string ERR_UNKNOWNCOMMAND(const std::string& str, const std::string& command) {
        return  "421 " + str + " " + command + " :Unknown command";
    }
    static std::string ERR_NEEDMOREPARAMS(const std::string& str, const std::string& command) {
        return  "461 " + str + " " + command + " :Not enough parameters";
    }
    static std::string ERR_TOOMANYCHANNELS(const std::string& str, const std::string& channel) {
        return  "405 " + str + " " + channel + " :You have joined too many channels";
    }
    static std::string ERR_NOTONCHANNEL(const std::string& str, const std::string& channel) {
        return  "442 " + str + " " + channel + " :You're not on that channel";
    }
    static std::string ERR_NOSUCHCHANNEL(const std::string& str, const std::string& channel) {
        return  "403 " + str + " " + channel + " :No such channel";
    }
    static std::string ERR_BADCHANNELKEY(const std::string& str, const std::string& channel) {
        return  "475 " + str + " " + channel + " :Cannot join channel (+k)";
    }
    static std::string ERR_CHANNELISFULL(const std::string& str, const std::string& channel) {
        return "471 " + str + " " + channel + " :Cannot join channel (+l)";
    }
    static std::string ERR_CANNOTSENDTOCHAN(const std::string& str, const std::string& channel) {
        return "404 " + str + " " + channel + " :Cannot send to channel";
    }
    static std::string ERR_CHANOPRIVSNEEDED(const std::string& str, const std::string& channel) {
        return "482 " + str + " " + channel + " :You're not channel operator";
    }
    static std::string ERR_NOSUCHNICK(const std::string& str, const std::string& nickname) {
        return "401 " + str + " " + nickname + " :No such nick/channel";
    }
    static std::string ERR_USERNOTINCHANNEL(const std::string& str, const std::string& nickname, const std::string& channel) {
        return "441 " + str + " " + nickname + " " + channel + " :They aren't on that channel";
    }
    /* Numeric Responses */
    static std::string RPL_WELCOME(const std::string& str) {
        return "001 " + str + " :Welcome " + str + " to the ft_irc network";
    }
    static std::string RPL_NAMREPLY(const std::string& str, const std::string& channel, const std::string& users) {
        return "353 " + str + " = " + channel + " :" + users;
    }
    static std::string RPL_ENDOFNAMES(const std::string& str, const std::string& channel) {
        return "366 " + str + " " + channel + " :End of /NAMES list.";
    }    
    /* Command Responses */   
    static std::string RPL_JOIN(const std::string& str, const std::string& channel) {
        return ":" + str + " JOIN :" + channel;
    }
    static std::string RPL_PART(const std::string& str, const std::string& channel) {
        return ":" + str + " PART :" + channel;
    }
    static std::string RPL_PING(const std::string& str, const std::string& token) {
        return ":" + str + " PONG :" + token;
    }
    static std::string RPL_PRIVMSG(const std::string& str, const std::string& target, const std::string& message) {
        return ":" + str + " PRIVMSG " + target + " :" + message;
    }
    static std::string RPL_NOTICE(const std::string& str, const std::string& target, const std::string& message) {
        return ":" + str + " NOTICE " + target + " :" + message;
    }
    static std::string RPL_QUIT(const std::string& str, const std::string& message) {
        return ":" + str + " QUIT :Quit: " + message;
    }
    static std::string RPL_KICK(const std::string& str, const std::string& channel, const std::string& target, const std::string& reason) {
        return ":" + str + " KICK " + channel + " " + target + " :" + reason;
    }
    static std::string RPL_MODE(const std::string& str, const std::string& channel, const std::string& modes, const std::string& args) {
        return ":" + str + " MODE " + channel + " " + modes + " " + args;
    }
};