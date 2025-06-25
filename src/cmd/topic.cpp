/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 16:37:01 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/25 17:59:07 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::TopicCommand(int client_fd, const std::vector<std::string> &tokens)
{
	Client *client = findClientByFd(client_fd);	// Search a Client
	
	if (!client)
	{
		std::cerr << "Error: Client not found for fd " << client_fd << std::endl;
		return;
	}

	if (!client->isRegistered())	// Verify that the client is registered
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}

	if (tokens.size() < 2)	// Check sufficient parameters
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "TOPIC");
		client->sendMessage(error);
		return;
	}
}