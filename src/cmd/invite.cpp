/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:02:08 by albelope          #+#    #+#             */
/*   Updated: 2025/06/30 12:10:13 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::InviteCommand(int client_fd, std::vector<std::string> &tokens) {
	if (tokens.size() != 3) {
		std::cout << "Error: RPL parameteres" << std::endl;
		return ;		
	}

	std::string nicknameToInvite = tokens[1];
	std::string channelName = tokens[2];

	if (nicknameToInvite.empty() || channelName.empty()) {
		std::cout << "Error: Invalid parameters" << std::endl;
		return;
	}

	Client* inviter = findClientByFd(client_fd);

	if (!inviter->isRegistered()) {
		std::cout << "u are not registerd in channel" << std::endl;
		return ;
	}

	Channel* channelToInvite = findChannelByName(channelName);
	
	if (!channelToInvite) {
		std::cout << "Channel no exist" << std::endl;
		return ;
	}
	
	if (!inviter->isInChannel(channelName)) {
		std::cout << "u are not in the channel" << std::endl;
		return ;
	}
	
	if (channelToInvite->isInviteOnly() && !channelToInvite->isOperator(inviter)) {
		std::cout << "Error: RPL imple" << std::endl;
		return;
	}
	Client* invitedClient = findClientByNickname(nicknameToInvite);

	if (!invitedClient) {
		std::cout << "User nickname not found" << std::endl;
		return ;
		
	}

	if (invitedClient->isInChannel(channelName)) {
		std::cout << "Error: User to invite is already in channel" << std::endl;
		return ; 
	}

	channelToInvite->addInvited(invitedClient);
	
	std::string reply;
	reply = ":"+ inviter->getNickName() + " " + invitedClient->getNickName() + " " + channelName + "\r\n";
	inviter->sendMessage(reply);

	std::string inviteMessage;
	inviteMessage = ":" + inviter->getNickName() + " INVITE " + invitedClient->getNickName() + " :" + channelName + "\r\n";
	invitedClient->sendMessage(inviteMessage);

}