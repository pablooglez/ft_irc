/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 16:45:52 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/17 18:12:48 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::NickCommand(int client_fd, const std::vector<std::string> &tokens)
{
	(void)client_fd;

	if (tokens.size() < 2)
	{
		std::cout << "NICK: insufficient parameters" << std::endl;
		return;
	}

	std::string nickname = tokens[1];
}