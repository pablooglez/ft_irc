/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:02:08 by albelope          #+#    #+#             */
/*   Updated: 2025/07/07 21:26:24 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::InviteCommand(int client_fd, std::vector<std::string> &tokens)
{
	Client* inviter = findClientByFd(client_fd);
	
	if(!inviter)
		return;

	if (tokens.size() != 3)
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), inviter->getNickName(), "INVITE");
		inviter->sendMessage(error);
		return ;
	}

	std::string nicknameToInvite = tokens[1];
	std::string channelName = tokens[2];

	if (nicknameToInvite.empty() || channelName.empty())
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), inviter->getNickName(), "INVITE");
		inviter->sendMessage(error);
		return;
	}

	if (!inviter->isRegistered())
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), inviter->getNickName());
		inviter->sendMessage(error);
		return;
	}

	Channel* channelToInvite = findChannel(channelName);
	
	if (!channelToInvite)
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), inviter->getNickName(), channelName);
		inviter->sendMessage(error);
		return;
	}
	
	if (!inviter->isInChannel(channelName))
	{
		std::string error = RPL::ERR_NOTONCHANNEL(getServerName(), inviter->getNickName(), channelName);
		inviter->sendMessage(error);
		return;
	}
	
	if (channelToInvite->isInviteOnly() && !channelToInvite->isOperator(inviter))
	{
		std::string error = RPL::ERR_CHANOPRIVSNEEDED(getServerName(), inviter->getNickName(), channelName);
		inviter->sendMessage(error);
		return;
	}

	Client* invitedClient = findClientByNickname(nicknameToInvite);

	if (!invitedClient)
	{
		std::string error = RPL::ERR_NOSUCHNICK(getServerName(), inviter->getNickName(), nicknameToInvite);
		inviter->sendMessage(error);
		return;
	}

	if (invitedClient->isInChannel(channelName))
	{
		std::string error = RPL::ERR_USERONCHANNEL(getServerName(), inviter->getNickName(), nicknameToInvite, channelName);
		inviter->sendMessage(error);
		return;
	}

	channelToInvite->addInvited(invitedClient);
	
	std::string reply = RPL::RPL_INVITING(getServerName(), inviter->getNickName(), nicknameToInvite, channelName);
	inviter->sendMessage(reply);

	std::string inviteMessage = ":" + inviter->getNickName() + " INVITE " + invitedClient->getNickName() + " :" + channelName + "\r\n";
	invitedClient->sendMessage(inviteMessage);
}