/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 22:44:58 by albelope          #+#    #+#             */
/*   Updated: 2025/07/03 17:41:06 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"


void    Server::KickCommand(int client_fd, std::vector<std::string> &tokens) {

    Client* kickingClient = findClientByFd(client_fd);

    if (!kickingClient->isRegistered()) {
        std::string error = RPL::ERR_NOTREGISTERED(getServerName(), kickingClient->getNickName());
        kickingClient->sendMessage(error);
        return;
    }

    if (tokens.size() != 3 && tokens.size() != 4) {
        std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), kickingClient->getNickName(), "KICK");
        kickingClient->sendMessage(error);
        return;
    }

    std::string channelName = tokens[1];
    std::string nickToKick = tokens[2];
    std::string msg = "No reason";

    if (tokens.size() == 4)
        msg = tokens[3];

    Channel* channelOfKick = findChannelByName(channelName);

    if (!channelOfKick) {
        std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), kickingClient->getNickName(), channelName);
        kickingClient->sendMessage(error);
        return;
    }

    if (!channelOfKick->isOperator(kickingClient)) {
        std::string error = RPL::ERR_CHANOPRIVSNEEDED(getServerName(), kickingClient->getNickName(), channelName);
        kickingClient->sendMessage(error);
        return;
    }

    if (!channelOfKick->hasClient(nickToKick)) {
        std::string error = RPL::ERR_USERNOTINCHANNEL(getServerName(), kickingClient->getNickName(), nickToKick, channelName);
        kickingClient->sendMessage(error);
        return;
    }

    Client *clientToKick = findClientByNickname(nickToKick);

    if (!channelOfKick->canKick(kickingClient, clientToKick)) {
        std::string error = RPL::ERR_CHANOPRIVSNEEDED(getServerName(), kickingClient->getNickName(), channelName);
        kickingClient->sendMessage(error);
        return;
    }

    std::string KickMessage;

    KickMessage = ":" + kickingClient->getNickName() + " KICK " + channelName +  " " + nickToKick; 

    if (!msg.empty())
        KickMessage = KickMessage + " :" + msg;

    channelOfKick->broadcast(KickMessage, NULL);

    if (channelOfKick->kickUser(kickingClient, clientToKick)) {
        clientToKick->removeChannel(channelOfKick);

        if (channelOfKick->isEmpty()) 
            _channels.erase(channelName);  
    }
}