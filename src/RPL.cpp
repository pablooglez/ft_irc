/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:29:47 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/19 17:03:35 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RPL.hpp"

std::string RPL::formatRPL(int code, const std::string &server, const std::string &nick,const std::string &message)
{
	std::ostringstream oss;

	oss << ":" << server << " " << code << " " << nick << " " << message << "\r\n";
	
	return (oss.str());
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