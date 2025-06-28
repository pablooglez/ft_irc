/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 22:44:58 by albelope          #+#    #+#             */
/*   Updated: 2025/06/28 23:29:39 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"


void    Server::KickCommand(int client_fd, std::vector<std::string> &tokens) {

    Client* client = findClientByFd(client_fd);

    if (!client->isRegistered()) {
        std::cout << "Error : Not registered" << std::endl;
        return ;
    }

    if (tokens.size() != 3 && tokens.size() != 4) {
        std::cout << "Error: Not correct parameters." << std::endl;
        return ;
    }

    std::string channel = tokens[1];
    std::string targetUser = tokens[2];
    std::string msg;

    if (tokens.size() == 4)
        msg = tokens[3];

    Channel* targetChannel = findChannelByName(channel);

    if (!targetChannel) {
        std::cout << "Error: channel not exsits" << std::endl;
        return ;
    }

    if (!targetChannel->isOperator(client)) {
        std::cout << "Error : is not operator" << std::endl;
        return ;
    }
    
    if (!targetChannel->hasClient(targetUser)) {
        std::cout << "Error : user not found" << std::endl;
        return ;
    }

    Client *targetClient = findClientByNickname(targetUser);

 
    

    
        
    

    

    
    

    
    
}