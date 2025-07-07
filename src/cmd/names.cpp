/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:02:02 by albelope          #+#    #+#             */
/*   Updated: 2025/06/30 13:04:15 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::NamesCommand(int client_fd, std::vector<std::string> &tokens) {
	
	Client* client = findClientByFd(client_fd);
	
	if (!client || !client->isRegistered()) {
        std::cout << "Erro:RPL IMPLE" << std::endl;
        return;
    }
	
	if (tokens.size() <= 1) {
		std::cout << "we need know the channel name" << std::endl;
		return ;
	}
	
	std::string channelName = tokens[1];

	if (!Channel::isValidChannelName(channelName)) {
        std::cout << "Error: Invalid channel name" << std::endl;
        return;
    }

	Channel* channel = findChannelByName(channelName);
	if (!channel)
    {
        client->sendMessage(RPL::RPL_ENDOFNAMES(getServerName(), client->getNickName(), channelName));
        return;
    }
    client->sendMessage(RPL::RPL_NAMREPLY(getServerName(), client->getNickName(),
						channel->getChannelName(), channel->getNamesReply()));
    client->sendMessage(RPL::RPL_ENDOFNAMES(getServerName(), client->getNickName(), channel->getChannelName()));
}

	