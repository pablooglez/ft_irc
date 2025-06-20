/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:29:47 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/20 20:42:57 by pablogon         ###   ########.fr       */
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
	std::string message = ":Welcome to the Internet Relay Network ";
	message += nick + "!" + user + "@" + host;
	return formatRPL(1, server, nick, message);
}

std::string RPL::RPL_YOURHOST(const std::string &server, const std::string &nick, const std::string &servername, const std::string &version)
{
	std::string message = ":Your host is ";
	message += servername + ", running version " + version;
	return formatRPL(2, server, nick, message);
}

std::string RPL::RPL_CREATED(const std::string &server, const std::string &nick, const std::string &date)
{
	std::string message = ":This server was created ";
	message += date;
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
