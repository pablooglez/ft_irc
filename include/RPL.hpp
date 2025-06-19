/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPL.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:12:25 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/19 16:50:42 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>

class	RPL
{
	public:
			// PASS
			static std::string ERR_NEEDMOREPARAMS(const std::string &server, const std::string &nick, const std::string &command);	// 461
			static std::string ERR_ALREADYREGISTERED(const std::string &server, const std::string &nick);	// 462
			static std::string ERR_PASSWDMISMATCH(const std::string &server, const std::string &nick);	// 464

	private:
			static std::string formatRPL(int code, const std::string &server, const std::string &nick, const std::string &message);
};