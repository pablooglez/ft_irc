/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 18:18:05 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/07 17:47:42 by pablogon         ###   ########.fr       */
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
			
			bool was_operator = channel->isOperator(client);	//Verify if the departing customer is an operator before removal

			channel->broadcastToOthers(quit_notification, client);	// Send notification to other channel users (not to the one leaving)

			channel->removeClient(client);	// Remove Client of channel

			if (was_operator && channel->getClientCount() > 0)	// If it was an operator and there are still users, promote the oldest one.
			{
				std::vector<Client*> remaining_clients = channel->getMembers();

				bool has_operator = false;
				
				for (size_t j = 0; j < remaining_clients.size(); ++j)
				{
					if (channel->isOperator(remaining_clients[j]))
					{
						has_operator = true;
						break;
					}
				}

				if (!has_operator && !remaining_clients.empty())	//If there are no operators, promote to the first (oldest) one.
				{
					Client* new_operator = remaining_clients[0];
					channel->addOperator(new_operator);
					
					// Notify operator of promotion
					std::string mode_msg = ":" + getServerName() + " MODE " + channel->getChannelName() + " +o " + new_operator->getNickName() + "\r\n";
					
					// Send to all channel users
					const std::vector<Client*> &channel_clients = channel->getMembers();

					for (std::vector<Client*>::const_iterator it = channel_clients.begin(); it != channel_clients.end(); ++it)
					{
						(*it)->sendMessage(mode_msg);
					}
				}
			}

			if (channel->getClientCount() == 0)		// If the channel is empty, delete it from the server.
			{
				_channels.erase(channel->getChannelName());
			}
		}
	}
	removeClient(client_fd);
}