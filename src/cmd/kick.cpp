/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 22:44:58 by albelope          #+#    #+#             */
/*   Updated: 2025/07/08 18:18:25 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::KickCommand(int client_fd, std::vector<std::string> &tokens)
{

	Client* kickingClient = findClientByFd(client_fd);

	if (!kickingClient->isRegistered())
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), kickingClient->getNickName());
		kickingClient->sendMessage(error);
		return;
	}

	if (tokens.size() < 3)
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), kickingClient->getNickName(), "KICK");
		kickingClient->sendMessage(error);
		return;
	}

	std::string channelName = tokens[1];

	if (channelName.empty() || channelName[0] != '#')
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), kickingClient->getNickName(), channelName);
		kickingClient->sendMessage(error);
		return;
	}

	std::string nickToKick = tokens[2];
	
	if (nickToKick.empty())
	{
		std::string error = RPL::ERR_NOSUCHNICK(getServerName(), kickingClient->getNickName(), nickToKick);
		kickingClient->sendMessage(error);
		return;
	}
	
	std::string msg = "";	// Improved kicking ratio management
	
	
	if (tokens.size() >= 4)
	{
		msg = tokens[3];

		if (!msg.empty() && msg[0] == ':')
			msg = msg.substr(1);

		for (size_t i = 4; i < tokens.size(); ++i)
			msg += " " + tokens[i];
	}

	if(msg.empty())
		msg = "No reason";

	Channel* channelOfKick = findChannel(channelName);

	if (!channelOfKick)
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), kickingClient->getNickName(), channelName);
		kickingClient->sendMessage(error);
		return;
	}

	if (!channelOfKick->hasClient(kickingClient->getNickName()))	// New: Verify that the Customer Executor is on the channel
	{
		std::string error = RPL::ERR_NOTONCHANNEL(getServerName(), kickingClient->getNickName(), channelName);
		kickingClient->sendMessage(error);
		return;
	}

	if (!channelOfKick->isOperator(kickingClient))
	{
		std::string error = RPL::ERR_CHANOPRIVSNEEDED(getServerName(), kickingClient->getNickName(), channelName);
		kickingClient->sendMessage(error);
		return;
	}

	Client *clientToKick = findClientByNickname(nickToKick);	// New: Verify that the nick exists on the server.

	if (!clientToKick)
	{
		std::string error = RPL::ERR_NOSUCHNICK(getServerName(), kickingClient->getNickName(), nickToKick);
		kickingClient->sendMessage(error);
		return;
	}

	if (!channelOfKick->hasClient(nickToKick))
	{
		std::string error = RPL::ERR_USERNOTINCHANNEL(getServerName(), kickingClient->getNickName(), nickToKick, channelName);
		kickingClient->sendMessage(error);
		return;
	}

	if (!channelOfKick->canKick(kickingClient, clientToKick))
	{
		std::string error = RPL::ERR_CHANOPRIVSNEEDED(getServerName(), kickingClient->getNickName(), channelName);
		kickingClient->sendMessage(error);
		return;
	}

	std::string KickMessage;

	KickMessage = ":" + kickingClient->getNickName() + " KICK " + channelName +  " " + nickToKick;

	if (!msg.empty())
		KickMessage = KickMessage + " :" + msg;

	channelOfKick->broadcast(KickMessage, NULL);

	if (channelOfKick->kickUser(kickingClient, clientToKick))
	{
		clientToKick->removeChannel(channelOfKick);
		
		if (channelOfKick->isEmpty())
			_channels.erase(channelName);
	}
}