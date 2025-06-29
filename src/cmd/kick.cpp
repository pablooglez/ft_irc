/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 22:44:58 by albelope          #+#    #+#             */
/*   Updated: 2025/06/29 14:46:00 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"


void    Server::KickCommand(int client_fd, std::vector<std::string> &tokens) {

    Client* kickingClient = findClientByFd(client_fd);

    if (!kickingClient->isRegistered()) {
        std::cout << "Error : Not registered" << std::endl;
        return ;
    }

    if (tokens.size() != 3 && tokens.size() != 4) {
        std::cout << "Error: Not correct parameters." << std::endl;
        return ;
    }

    std::string channelName = tokens[1];
    std::string nickToKick = tokens[2];
    std::string msg = "No reason";

    if (tokens.size() == 4)
        msg = tokens[3];

    Channel* channelOfKick = findChannelByName(channelName);

    if (!channelOfKick) {
        std::cout << "Error: channel not exsits" << std::endl;
        return ;
    }

    if (!channelOfKick->isOperator(kickingClient)) {
        std::cout << "Error : is not operator" << std::endl;
        return ;
    }
    
    if (!channelOfKick->hasClient(nickToKick)) {
        std::cout << "Error : user not found" << std::endl;
        return ;
    }

    Client *clientToKick = findClientByNickname(nickToKick);

    if (!channelOfKick->canKick(kickingClient, clientToKick)) {
        std::cout << "Error : cannot kick this user" << std::endl;
        return ;
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