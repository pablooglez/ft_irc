/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 18:09:47 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/24 23:09:40 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::PrivmsgCommand(int client_fd, const std::vector<std::string> &tokens)
{
	Client *client = findClientByFd(client_fd);		// Search Client

	if (!client)
	{
		std::cerr << "Error: Client not found for fd " << client_fd << std::endl;
		return;
	}

	if (!client->isRegistered())	 // Verify that the client is registered
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}

	if (tokens.size() < 2)	// Check sufficient parameters
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "PRIVMSG");
		client->sendMessage(error);
		return;
	}

	if (tokens.size() < 3)	// Check sufficient parameters
	{
		std::string error = RPL::ERR_NOTEXTTOSEND(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}

	std::string target = tokens[1];
	std::string message = tokens[2];

	for (size_t i = 3; i < tokens.size(); ++i)	// Reconstruct complete message (may have spaces)
	{
		message += " " + tokens[i];
	}

	if (!message.empty() && message[0] == ':')	// Remove ‘:’ from startup if it exists
	{
		message = message.substr(1);
	}

	if (message.empty())	// Verify that the message is not empty after processing
	{
		std::string error = RPL::ERR_NOTEXTTOSEND(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}

	if (target[0] == '#')
	{
		Channel *channel = findChannel(target);

		if (!channel)
		{
			std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), client->getNickName(), target);
			client->sendMessage(error);
			return;
		}

		if (!client->isInChannel(channel))
		{
			std::string error = RPL::ERR_CANNOTSENDTOCHAN(getServerName(), client->getNickName(), target);
			client->sendMessage(error);
			return;
		}

		// Send to ALL users in the channel EXCEPT the sender
		std::string privmsg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " PRIVMSG " + target + " :" + message + "\r\n";	// Send to ALL users in the channel EXCEPT the sender

		const std::vector<Client*>& channel_clients = channel->getClients();
		for (std::vector<Client*>::const_iterator it = channel_clients.begin(); it != channel_clients.end(); ++it)
		{
			if ((*it)->getFd() != client_fd)  // Don't send to sender
			{
				(*it)->sendMessage(privmsg);
			}
		}

		std::cout << "PRIVMSG from " << client->getNickName() << " to channel " << target << ": " << message << std::endl;
	}
	else
	{
		// Private message to user
		Client* target_client = findClientByNick(target);
		if (!target_client)
		{
			std::string error = RPL::ERR_NOSUCHNICK(getServerName(), client->getNickName(), target);
			client->sendMessage(error);
			return;
		}

		std::string privmsg = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " PRIVMSG " + target + " :" + message + "\r\n";
		target_client->sendMessage(privmsg);

		std::cout << "PRIVMSG from " << client->getNickName() << " to user " << target << ": " << message << std::endl;
	}
}