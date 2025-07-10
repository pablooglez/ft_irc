/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 16:37:01 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/10 21:22:23 by pablogon         ###   ########.fr       */
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

	std::string channel_name = tokens[1];	// Get channel name

	if (channel_name[0] != '#')
	{
		channel_name = '#' + channel_name;
	}

	Channel *channel = findChannel(channel_name);	// Search the channel
	if (!channel)
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), client->getNickName(), channel_name);
		client->sendMessage(error);
		return;
	}

	if (!client->isInChannel(channel))	// Verify that the user is in the channel
	{
		std::string error = RPL::ERR_NOTONCHANNEL(getServerName(), client->getNickName(), channel_name);
		client->sendMessage(error);
		return;
	}

	if (tokens.size() == 2)
	{
		if (channel->getTopic().empty())
		{
			std::string response = RPL::RPL_NOTOPIC(getServerName(), client->getNickName(), channel_name);
			client->sendMessage(response);
		}
		else
		{
			std::string response = RPL::RPL_TOPIC(getServerName(), client->getNickName(), channel_name, channel->getTopic());
			client->sendMessage(response);
		}
	}
	else
	{
		if (!channel->canSetTopic(client))	// Verify permissions based on channel mode
		{
			std::string error = RPL::ERR_CHANOPRIVSNEEDED(getServerName(), client->getNickName(), channel_name);
			client->sendMessage(error);
			return;
		}

		std::string new_topic = tokens[2];	// Rebuild new topic

		for(size_t i = 3; i < tokens.size(); ++i)
		{
			new_topic += " " + tokens[i];
		}

		if (!new_topic.empty() && new_topic[0] == ':')	// Remove ‘:’ from the start if it exists
		{
			new_topic = new_topic.substr(1);
		}

		channel->setTopic(new_topic);	// Establish new topic in the channel

		std::string topic_change = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " TOPIC " + channel_name + " :" + new_topic + "\r\n";
	
		const std::vector<Client*> &channel_clients = channel->getMembers();
		for (std::vector<Client*>::const_iterator it = channel_clients.begin(); it != channel_clients.end(); ++it)
		{
			(*it)->sendMessage(topic_change);
		}
	}
}