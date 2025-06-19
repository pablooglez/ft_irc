/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 18:18:05 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/19 21:31:16 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::QuitCommand(int client_fd, const std::vector<std::string> &tokens)
{
	(void) tokens;

	Client *client = findClientByFd(client_fd);	// Search a Client

	if (!client)
	{
		removeClient(client_fd);
		return;
	}

	std::string quit_message = "Client Quit";

	std::cout << "Client " << client->getNickName() << " (" << client_fd << ") quit:" << quit_message << std::endl;

	if (!client->getNickName().empty())
	{
		std::string quit_notification = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " QUIT :" + quit_message + "\r\n";

		const std::vector<Channel*> &channels = client->getChannels();	// Get client channel list

		for (size_t i = 0; i < channels.size(); ++i)
		{
			Channel *channel = channels[i];

			channel->broadcastToOthers(quit_notification, client);	// Send notification to other channel users (not to the one leaving)

			channel->removeClient(client);	// Remove Client of channel

			if (channel->getClientCount() == 0)		// If the channel is empty, delete it from the server.
			{
				_channels.erase(channel->getName());
			}
		}
	}
	removeClient(client_fd);
}