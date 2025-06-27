/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 17:51:21 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/27 20:30:32 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::UserCommand(int client_fd, const std::vector<std::string> &tokens)
{
	Client *client = findClientByFd(client_fd);		// Search a Client

	if (!client)
	{
		std::cerr << "Error: Client not found for fd" << client_fd << std::endl;
		return;
	}

	if (tokens.size() < 2)	// Check sufficient parameters (only username required)
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), getClientNick(client_fd), "USER");
		client->sendMessage(error);
		return;
	}

	if (!client->isAuthenticated())	// USER requires previous authentication with PASS
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), getClientNick(client_fd));
		client->sendMessage(error);
		return;
	}

	if (!client->hasNickName())	// USER requires that NICK has been set previously
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), getClientNick(client_fd), "USER");
		client->sendMessage(error);
		return;
	}

	if (client->isRegistered())	// USER cannot be used if already registered
	{
		std::string error = RPL::ERR_ALREADYREGISTERED(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}

	std::string username = tokens[1];	// Extract username (only first parameter)

	// Validate that username is not empty
	if (username.empty())
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), getClientNick(client_fd), "USER");
		client->sendMessage(error);
		return;
	}

	client->setUserName(username);	// Update client information
	client->setHasUserInfo(true);

	client->updateRegistrationStatus();	// Update registration status

	std::cout << "Client " << client_fd << " set username: " << username << std::endl;

	if (client->isRegistered())	// If now fully registered, send welcome messages
	{
		std::cout << "Client " << client_fd << " is now fully registered!" << std::endl;
		sendWelcomeMessages(client_fd);
	}
}
