/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:29:47 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/03 17:45:51 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/RPL.hpp"

std::string RPL::formatRPL(int code, const std::string &server, const std::string &nick,const std::string &message)
{
	std::ostringstream oss;

	oss << ":" << server << " " << code << " " << nick << " " << message << "\r\n";
	
	return (oss.str());
}

// WELCOME MESSAGES
std::string RPL::RPL_WELCOME(const std::string &server, const std::string &nick, const std::string &user, const std::string &host)
{
    std::string message = ":Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host;
    return formatRPL(1, server, nick, message);
}

std::string RPL::RPL_YOURHOST(const std::string &server, const std::string &nick, const std::string &servername, const std::string &version)
{
    std::string message = ":Your host is " + servername + ", running version " + version;
    return formatRPL(2, server, nick, message);
}

std::string RPL::RPL_CREATED(const std::string &server, const std::string &nick, const std::string &date)
{
    std::string message = ":This server was created " + date;
    return formatRPL(3, server, nick, message);
}

std::string RPL::RPL_MYINFO(const std::string &server, const std::string &nick, const std::string &servername, const std::string &version, const std::string &user_modes, const std::string &channel_modes)
{
    std::string message = servername + " " + version + " " + user_modes + " " + channel_modes;
    return formatRPL(4, server, nick, message);
}

// PASS
std::string RPL::ERR_NEEDMOREPARAMS(const std::string &server, const std::string &nick, const std::string &command)
{
	return formatRPL(461, server, nick, command + " :Not enough parameters");
}

std::string RPL::ERR_ALREADYREGISTERED(const std::string &server, const std::string &nick)
{
	return formatRPL(462, server, nick, ":You may not reregister");
}

std::string RPL::ERR_PASSWDMISMATCH(const std::string &server, const std::string &nick)
{
	return formatRPL(464, server, nick, ":Password incorrect");
}

// NICK
std::string RPL::ERR_NONICKNAMEGIVEN(const std::string &server, const std::string &nick)
{
	return formatRPL(431, server, nick, ":No nickname given");
}

std::string RPL::ERR_ERRONEUSNICKNAME(const std::string &server, const std::string &nick, const std::string &bad_nick)
{
	return formatRPL(432,server, nick, bad_nick + " :Erroneous nickname");
}

std::string RPL::ERR_NICKNAMEINUSE(const std::string &server, const std::string &nick, const std::string &used_nick)
{
	return formatRPL(433, server, nick, used_nick + " :Nickname is already in use");
}

// USER
std::string RPL::ERR_NOTREGISTERED(const std::string &server, const std::string &nick)
{
	return formatRPL(451, server, nick, ":You have not registered");
}

// JOIN
std::string RPL::RPL_NOTOPIC(const std::string &server, const std::string &nick, const std::string &channel)
{
    return formatRPL(331, server, nick, channel + " :No topic is set");
}

std::string RPL::RPL_TOPIC(const std::string &server, const std::string &nick, const std::string &channel, const std::string &topic)
{
    return formatRPL(332, server, nick, channel + " :" + topic);
}

std::string RPL::RPL_NAMREPLY(const std::string &server, const std::string &nick, const std::string &channel, const std::string &names)
{
    return formatRPL(353, server, nick, "= " + channel + " :" + names);
}

std::string RPL::RPL_ENDOFNAMES(const std::string &server, const std::string &nick, const std::string &channel)
{
    return formatRPL(366, server, nick, channel + " :End of /NAMES list");
}

std::string RPL::ERR_NOSUCHCHANNEL(const std::string &server, const std::string &nick, const std::string &channel)
{
	return formatRPL(403, server, nick, channel + " :No such channel");
}

std::string RPL::ERR_CHANNELISFULL(const std::string &server, const std::string &nick, const std::string &channel)
{
	return formatRPL(471, server, nick, channel + " :Cannot join channel (+l)");
}

std::string RPL::ERR_INVITEONLYCHAN(const std::string &server, const std::string &nick, const std::string &channel)
{
	return formatRPL(473, server, nick, channel + " :Cannot join channel (+i)");
}

std::string RPL::ERR_BADCHANNELKEY(const std::string &server, const std::string &nick, const std::string &channel)
{
	return formatRPL(475, server, nick, channel + " :Cannot join channel (+k)");
}

// PRIVMSG
std::string RPL::ERR_NOSUCHNICK(const std::string &server, const std::string &nick, const std::string &target)
{
	return formatRPL(401, server, nick, target + " :No such nick/channel");
}

std::string RPL::ERR_NOSUCHSERVER(const std::string &server, const std::string &nick, const std::string &servername)
{
	return formatRPL(402, server, nick, servername + " :No such server");
}

std::string RPL::ERR_CANNOTSENDTOCHAN(const std::string &server, const std::string &nick, const std::string &channel)
{
	return formatRPL(404, server, nick, channel + " :Cannot send to channel");
}

std::string RPL::ERR_NOTEXTTOSEND(const std::string &server, const std::string &nick)
{
	return formatRPL(412, server, nick, " :No text to send");
}

// TOPIC
std::string RPL::ERR_CHANOPRIVSNEEDED(const std::string &server, const std::string &nick, const std::string &channel)
{
	return formatRPL(482, server, nick, channel + " :You're not channel operator");
}

std::string RPL::ERR_NOTONCHANNEL(const std::string &server, const std::string &nick, const std::string &channel)
{
	return formatRPL(442, server, nick, channel + " :You're not on that channel");
}

std::string RPL::RPL_CHANNELMODEIS(const std::string &server, const std::string &nick, const std::string &channel, const std::string &modes)
{
    return formatRPL(324, server, nick, channel + " " + modes);
}

std::string RPL::RPL_LIST(const std::string &server, const std::string &nick, const std::string &channel, const std::string &visible_users, const std::string &topic)
{
    return formatRPL(322, server, nick, channel + " " + visible_users + " :" + topic);
}

std::string RPL::RPL_LISTEND(const std::string &server, const std::string &nick)
{
    return formatRPL(323, server, nick, ":End of /LIST command");
}

std::string RPL::RPL_INFO(const std::string &server, const std::string &nick, const std::string &info)
{
    return formatRPL(371, server, nick, info);
}

std::string RPL::RPL_ENDOFINFO(const std::string &server, const std::string &nick, const std::string &message)
{
    return formatRPL(374, server, nick, message);
}

// KICK
std::string RPL::ERR_USERNOTINCHANNEL(const std::string &server, const std::string &nick, const std::string &target, const std::string &channel) {
    return formatRPL(441, server, nick, target + " " + channel + " :They aren't on that channel");
}

std::string RPL::ERR_UNKNOWNMODE(const std::string& server, const std::string& nick, const std::string& modechar) {
    return ":" + server + " 472 " + nick + " " + modechar + " :is unknown mode char to me\r\n";
}

// INVITE
std::string RPL::RPL_INVITING(const std::string &server, const std::string &nick, const std::string &target, const std::string &channel) {
    return formatRPL(341, server, nick, target + " " + channel);
}

std::string RPL::ERR_USERONCHANNEL(const std::string &server, const std::string &nick, const std::string &target, const std::string &channel) {
    return formatRPL(443, server, nick, target + " " + channel + " :is already on channel");
}