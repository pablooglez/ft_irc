/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:12:25 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/20 20:43:16 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>

class	RPL
{
	public:
	
			// WELCOME MESSAGES
			static std::string RPL_WELCOME(const std::string &server, const std::string &nick, const std::string &user, const std::string &host);	// 001
			static std::string RPL_YOURHOST(const std::string &server, const std::string &nick, const std::string &servername, const std::string &version);	// 002
			static std::string RPL_CREATED(const std::string &server, const std::string &nick, const std::string &date);	// 003
			static std::string RPL_MYINFO(const std::string &server, const std::string &nick, const std::string &servername, const std::string &version, const std::string &user_modes, const std::string &channel_modes);	// 004
			
			// PASS
			static std::string ERR_NEEDMOREPARAMS(const std::string &server, const std::string &nick, const std::string &command);	// 461
			static std::string ERR_ALREADYREGISTERED(const std::string &server, const std::string &nick);	// 462
			static std::string ERR_PASSWDMISMATCH(const std::string &server, const std::string &nick);	// 464

			// NICK
			static std::string ERR_NONICKNAMEGIVEN(const std::string &server, const std::string &nick);	// 431
			static std::string ERR_ERRONEUSNICKNAME(const std::string &server, const std::string &nick, const std::string &bad_nick);	// 432
			static std::string ERR_NICKNAMEINUSE(const std::string &server, const std::string &nick, const std::string &used_nick);	// 433

			// USER
			static std::string ERR_NOTREGISTERED(const std::string &server, const std::string &nick);	// 451

	private:
			static std::string formatRPL(int code, const std::string &server, const std::string &nick, const std::string &message);
};