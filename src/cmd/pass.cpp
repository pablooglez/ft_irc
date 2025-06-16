/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 20:29:30 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/16 20:51:00 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::PassCommand(int client_fd, const std::vector<std::string> &tokens)
{

	if (tokens.size() < 2)
	{
		std::cout << "PASS: insufficient parameters" << std::endl;
		return;
	}

	std::string provided_password = tokens[1];
	std::cout << "Client" << client_fd << "trying to authenticate with: " << provided_password << std::endl;

	if (provided_password == this->_password)
		std::cout << "Successful authentication for client" << client_fd << std::endl;
	else
		std::cout << "Authentication failed for client" << client_fd << std::endl;
}