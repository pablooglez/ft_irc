/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 12:15:43 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/07 16:41:46 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::JoinCommand(int client_fd, const std::vector<std::string> &tokens)
{
	Client *client = findClientByFd(client_fd);		//Search a Client
	
	if (!client)
	{
		std::cerr << "Error: Client not found for fd " << client_fd << std::endl;
		return;
	}

	if (!client->isRegistered())
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}

	if (tokens.size() < 2)
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "JOIN");
		client->sendMessage(error);
		return;
	}

	std::string channel_name = tokens[1];
	
	if (channel_name.empty() || channel_name[0] != '#')	// Channel name MUST start with # - no auto-correction
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), client->getNickName(), channel_name);
		client->sendMessage(error);
		return;
	}

	// Validate channel name
	if (!Channel::isValidChannelName(channel_name))
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), client->getNickName(), channel_name);
		client->sendMessage(error);
		return;
	}

	// Find or create the channel
	Channel *channel = findOrCreateChannel(channel_name);
	if (!channel)
	{
		std::cerr << "Error: Failed to create/find channel " << channel_name << std::endl;
		return;
	}

	// Check if user is already in the channel
	if (channel->hasClient(client->getNickName())) // pablo tiene otra distitna
	{
		// User is already in channel, no error in IRC protocol, just ignore
		return;
	}

	// Check if user can join (password, invite-only, etc.)
	std::string password = (tokens.size() >= 3) ? tokens[2] : "";
	if (!channel->canJoin(client, password))
	{
		// Send appropriate error based on channel restrictions
		if (channel->isInviteOnly() && !channel->isInvited(client))
		{
			std::string error = ":localhost 473 " + client->getNickName() + " " + channel_name + " :Cannot join channel (+i)\r\n";
			client->sendMessage(error);
		}
		else if (channel->hasPassword() && (password.empty() || password != channel->getPassword()))
		{
			std::string error = ":localhost 475 " + client->getNickName() + " " + channel_name + " :Cannot join channel (+k)\r\n";
			client->sendMessage(error);
		}
		else if (channel->hasUserLimit() && channel->getClientCount() >= channel->getUserLimit())
		{
			std::string error = ":localhost 471 " + client->getNickName() + " " + channel_name + " :Cannot join channel (+l)\r\n";
			client->sendMessage(error);
		}
		return;
	}

	// Add user to channel
	if (!channel->addMember(client)) //pablo tiene addClient
	{
		std::cerr << "Error: Failed to add client to channel " << channel_name << std::endl;
		return;
	}

	// Add channel to client's list
	client->addChannel(channel);

	// If this is the first user, make them operator
	if (channel->getClientCount() == 1)
	{
		channel->addOperator(client);
		std::cout << "Client " << client->getNickName() << " is now operator of " << channel_name << std::endl;
	}

	// Send JOIN confirmation to the user
	std::string join_response = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " JOIN " + channel_name + "\r\n";
	client->sendMessage(join_response);

	// Broadcast JOIN to all other users in the channel
	const std::vector<Client*> &channel_clients = channel->getMembers();
	for (std::vector<Client*>::const_iterator it = channel_clients.begin(); it != channel_clients.end(); ++it)
	{
		(*it)->sendMessage(join_response);
	}
	
	// Send topic information
	if (channel->getTopic().empty())
	{
		std::string topic_response = RPL::RPL_NOTOPIC(getServerName(), client->getNickName(), channel_name);
		client->sendMessage(topic_response);
	}
	else
	{
		std::string topic_response = RPL::RPL_TOPIC(getServerName(), client->getNickName(), channel_name, channel->getTopic());
		client->sendMessage(topic_response);
	}

	// Send names list
	std::string names_list = channel->getNamesReply();
	std::string names_response = RPL::RPL_NAMREPLY(getServerName(), client->getNickName(), channel_name, names_list);
	client->sendMessage(names_response);

	// End of names list
	std::string end_names = RPL::RPL_ENDOFNAMES(getServerName(), client->getNickName(), channel_name);
	client->sendMessage(end_names);

	std::cout << "Client " << client->getNickName() << " joined channel " << channel_name 
			  << " (total users: " << channel->getClientCount() << ")" << std::endl;
}