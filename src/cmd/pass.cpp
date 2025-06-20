/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 20:29:30 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/20 21:36:50 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::PassCommand(int client_fd, const std::vector<std::string> &tokens)
{
	Client *client = findClientByFd(client_fd);	// Search a Client

	if (!client)
	{
		std::cerr << "Error: Client not found for fd" << client_fd << std::endl;
		return;
	}

	if (tokens.size() < 2)	// Check sufficient parameters
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), getClientNick(client_fd), "PASS");
		client->sendMessage(error);
		return;
	}

	if (client->isRegistered())	// Check if it's registered
	{
		std::string error = RPL::ERR_ALREADYREGISTERED(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}

	std::string provided_password = tokens[1];

	if (provided_password == this->_password)
	{
		client->setAuthenticated(true);
		std::cout << "Client" << client_fd << "authenticated successfully" << std::endl;

		// Send simple success message
		std::string next_instruction = ":localhost PRIVMSG * :Password accepted! Now set your nickname: NICK <your_nickname>\r\n";
		client->sendMessage(next_instruction);
	}
	else
	{
		std::string error = RPL::ERR_PASSWDMISMATCH(getServerName(), getClientNick(client_fd));
		client->sendMessage(error);
		std::cout << "Client" << client_fd << "failed authentication" << std::endl;
	}
}