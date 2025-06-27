/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 20:29:30 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/27 20:22:54 by pablogon         ###   ########.fr       */
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

	if (client->isAuthenticated())	// Check if already authenticated with PASS
	{
		std::string error = RPL::ERR_ALREADYREGISTERED(getServerName(), getClientNick(client_fd));
		client->sendMessage(error);
		return;
	}

	std::string provided_password = tokens[1];

	if (provided_password == this->_password)
	{
		client->setAuthenticated(true);
		std::cout << "Client" << client_fd << "authenticated successfully" << std::endl;
	}
	else
	{
		std::string error = RPL::ERR_PASSWDMISMATCH(getServerName(), getClientNick(client_fd));
		client->sendMessage(error);
		std::cout << "Client" << client_fd << "failed authentication" << std::endl;
	}
}