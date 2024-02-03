#pragma once

#include <ctime>
#include <iostream>
#include <string>

class IRCResponse {
      public:
	static std::string ERR_NOTREGISTERED(const std::string &str) {
		return "451 " + str + " :You have not registered";
	}
	static std::string ERR_ALREADYREGISTERED(const std::string &str) {
		return "462 " + str + " :You may not reregister";
	}
	static std::string ERR_PASSWDMISMATCH(const std::string &str) {
		return str + " :Password is incorrect";
	}
	static std::string ERR_NONICKNAMEGIVEN(const std::string &str) {
		return "431 " + str + " :No nickname given";
	}
	static std::string ERR_NICKNAMEINUSE(const std::string &str) {
		return "433 " + str + " " + str +
		       " :Nickname is already in use";
	}
	static std::string ERR_UNKNOWNCOMMAND(const std::string &str,
					      const std::string &command) {
		return "421 " + str + " " + command + " :Unknown command";
	}
	static std::string ERR_NEEDMOREPARAMS(const std::string &str,
					      const std::string &command) {
		return "461 " + str + " " + command + " :Not enough parameters";
	}
	static std::string ERR_TOOMANYCHANNELS(const std::string &str,
					       const std::string &channel) {
		return "405 " + str + " " + channel +
		       " :You have joined too many channels";
	}
	static std::string ERR_NOTONCHANNEL(const std::string &str,
					    const std::string &channel) {
		return "442 " + str + " " + channel +
		       " :You're not on that channel";
	}
	static std::string ERR_NOSUCHCHANNEL(const std::string &str,
					     const std::string &channel) {
		return "403 " + str + " " + channel + " :No such channel";
	}
	static std::string ERR_BADCHANNELKEY(const std::string &str,
					     const std::string &channel) {
		return "475 " + str + " " + channel +
		       " :Cannot join channel (+k)";
	}
	static std::string ERR_CHANNELISFULL(const std::string &str,
					     const std::string &channel) {
		return "471 " + str + " " + channel +
		       " :Cannot join channel (+l)";
	}
	static std::string ERR_CANNOTSENDTOCHAN(const std::string &str,
						const std::string &channel) {
		return "404 " + str + " " + channel +
		       " :Cannot send to channel";
	}
	static std::string ERR_CHANOPRIVSNEEDED(const std::string &str,
						const std::string &channel) {
		return "482 " + str + " " + channel +
		       " :You're not channel operator";
	}
	static std::string ERR_NOSUCHNICK(const std::string &str,
					  const std::string &nickname) {
		return "401 " + str + " " + nickname + " :No such nick/channel";
	}
	static std::string ERR_USERNOTINCHANNEL(const std::string &str,
						const std::string &nickname,
						const std::string &channel) {
		return "441 " + str + " " + nickname + " " + channel +
		       " :They aren't on that channel";
	}
	/* Numeric Responses */
	static std::string RPL_WELCOME(const std::string &str) {
		return "001 " + str + " :Welcome " + str +
		       " to the ft_irc network";
	}
	static std::string RPL_NAMREPLY(const std::string &str,
					const std::string &channel,
					const std::string &users) {
		return "353 " + str + " = " + channel + " :" + users;
	}
	static std::string RPL_ENDOFNAMES(const std::string &name,
					  const std::string &ch) {
		return "366 " + name +  " " + ch + " :End of /NAMES list";
	}
	/* Command Responses */
	static std::string RPL_JOIN(const std::string &str,
				    const std::string &channel) {
		return ":" + str + " JOIN :" + channel;
	}
	static std::string RPL_PART(const std::string &str,
				    const std::string &channel) {
		return ":" + str + " PART :" + channel;
	}
	static std::string RPL_PING(const std::string &str,
				    const std::string &token) {
		return ":" + str + " PONG :" + token;
	}
	static std::string RPL_PRIVMSG(const std::string &str,
				       const std::string &target,
				       const std::string &message) {
		return ":" + str + " PRIVMSG " + target + " :" + message;
	}
	static std::string RPL_NOTICE(const std::string &str,
				      const std::string &target,
				      const std::string &message) {
		return ":" + str + " NOTICE " + target + " :" + message;
	}
	static std::string RPL_QUIT(const std::string &str,
				    const std::string &message) {
		return ":" + str + " QUIT :Quit: " + message;
	}
	static std::string RPL_KICK(const std::string &str,
				    const std::string &channel,
				    const std::string &target,
				    const std::string &reason) {
		return ":" + str + " KICK " + channel + " " + target + " :" +
		       reason;
	}
	static std::string RPL_MODE(const std::string &prefix,
				    const std::string &channel,
				    const std::string &target) {
		return ":" + prefix + " MODE " + channel + " " + target;
	}
	static std::string ERR_ERRONEUSNICKNAME(const std::string &source,
						const std::string &command) {
		return "432 " + source + " " + command + " :Erroneous nickname";
	}
	static std::string ERR_USERONCHANNEL(const std::string &user,
					     const std::string &channel) {
		return "443 " + user + " " + channel + ":is already on channel";
	}
	static std::string ERR_BADCHANMASK(const std::string &source,
					   const std::string &command) {
		return "476 " + source + " " + command + " :Bad Channel Mask";
	}
	static std::string ERR_INVITEONLYCHAN(const std::string &source,
					      const std::string &command) {
		return "473 " + source + " " + command +
		       " :Cannot join channel (+i)";
	}
	static std::string RPL_CHANNELMODEIS(const std::string &nickname,
					     const std::string &channel,
					     const std::string &modes) {
		return "324 " + nickname + " " + channel + " " + modes;
	}
	static std::string ERR_KEYSET(const std::string &source,
				      const std::string &command) {
		return "467 " + source + " " + command +
		       " :Channel key already set";
	}
	static std::string RPL_MSG(const std::string &prefix,
				   const std::string &command,
				   const std::string &target,
				   const std::string &massage) {
		return ":" + prefix + " " + command + " " + target + " :" +
		       massage;
	}
	static std::string ERR_UNKNOWNMODE(const std::string &source,
					   const std::string &command,
					   const std::string &msg) {
		return "472 " + source + " " + command + " " + msg;
	}

	static std::string ERR_NOCHANMODES(const std::string &channelName) {
		return "477 # " + channelName +
		       " :TOPIC mode is OFF ('MODE <channel> +t' is needed)";
	}

	static std::string RPL_NOTOPIC(const std::string &channelName) {
		return "331 # " + channelName;
	}
	static std::string RPL_TOPIC(const std::string &channelName,
				     const std::string &topic) {
		return "332 # " + channelName + " :" + topic;
	}
	static std::string ERR_NOTEXTTOSEND(const std::string &source) {
		return "412 " + source + " :No text to send";
	}
	static std::string RPL_INVITE(const std::string &prefix,
				      const std::string &target,
				      const std::string &channel) {
		return ":" + prefix + " INVITE " + target + " " + channel;
	}
	static std::string RPL_INVITING(const std::string &name,
					const std::string &nick,
					const std::string &ch) {
		return "341 " + name + " " + nick + " " + ch;
	}
	static std::string
	RPL_WHOREPLY(const std::string &name, const std::string &ch,
		     const std::string &us, const std::string &hst,
		     const std::string &nick, const std::string &fl,
		     const std::string &re) {
		return "352 " + name + " " + ch + " " + us + " " + hst +
		       " ft_irc " + nick + " " + fl + " :" + re;
	}
	static std::string RPL_ENDOFWHO(const std::string &name,
					const std::string &source) {
		return "315 " + name + " " + source + " :End of WHO list";
	}
	static std::string RPL_NICKCHANGE(const std::string& oldNickname, const std::string& newNickname) {
        return oldNickname + " is now known as " + newNickname;
    }
};