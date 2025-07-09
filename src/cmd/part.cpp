/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 20:19:00 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/09 20:48:24 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::PartCommand(int client_fd, std::vector<std::string> &tokens)
{
	Client* client = findClientByFd(client_fd);

	if (!client)
		return;

	if (!client->isRegistered())
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}

	if (tokens.size() < 2)
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "PART");
		client->sendMessage(error);
		return;
	}

	std::string channelName = tokens[1];
	std::string partMessage = "";

	if (tokens.size() >= 3)	// Extract part message if provided
	{
		partMessage = tokens[2];

		if (!partMessage.empty() && partMessage[0] == ':')	// Remove leading : if present
			partMessage = partMessage.substr(1);

		for (size_t i = 3; i < tokens.size(); ++i)	// Concatenate remaining tokens
			partMessage += " " + tokens[i];
	}

	if (partMessage.empty())
		partMessage = client->getNickName();

	if (channelName.empty() || channelName[0] != '#')	// Validate channel name
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), client->getNickName(), channelName);
		client->sendMessage(error);
		return;
	}

	Channel* channel = findChannel(channelName);

	if (!channel)
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), client->getNickName(), channelName);
		client->sendMessage(error);
		return;
	}

	if (!channel->hasClient(client->getNickName()))
	{
		std::string error = RPL::ERR_NOTONCHANNEL(getServerName(), client->getNickName(), channelName);
		client->sendMessage(error);
		return;
	}

	std::string partBroadcast = ":" + client->getNickName() + " PART " + channelName + " :" + partMessage + "\r\n";	// Create PART message to broadcast

	channel->broadcast(partBroadcast, NULL);	// Broadcast to all channel members (including the leaving user)

	channel->removeClient(client);	// Remove client from channel
	client->removeChannel(channel);

	if (channel->isEmpty())	// Remove channel if empty
		_channels.erase(channelName);

	std::cout << "Client " << client->getNickName() << " parted from channel " << channelName << std::endl;
}
