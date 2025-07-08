/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:02:02 by albelope          #+#    #+#             */
/*   Updated: 2025/07/08 19:09:59 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::NamesCommand(int client_fd, std::vector<std::string> &tokens)
{
	Client* client = findClientByFd(client_fd);

	if (!client)
	{
		return;
	}

	if (!client->isRegistered())
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}
	
	if (tokens.size() <= 1)
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "NAMES");
		client->sendMessage(error);
		return;
	}
	
	std::string channelName = tokens[1];

	if (!Channel::isValidChannelName(channelName))
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), client->getNickName(), channelName);
		client->sendMessage(error);
		return;
	}

	Channel* channel = findChannel(channelName);

	if (!channel)
	{
		client->sendMessage(RPL::RPL_ENDOFNAMES(getServerName(), client->getNickName(), channelName));
		return;
	}
	client->sendMessage(RPL::RPL_NAMREPLY(getServerName(), client->getNickName(), channel->getChannelName(), channel->getNamesReply()));
	client->sendMessage(RPL::RPL_ENDOFNAMES(getServerName(), client->getNickName(), channel->getChannelName()));
}