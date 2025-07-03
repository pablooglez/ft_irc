/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:02:06 by albelope          #+#    #+#             */
/*   Updated: 2025/07/03 17:57:15 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
#include <sstream>

void Server::ListCommand(int client_fd, std::vector<std::string> &tokens)
{
    (void)tokens;
    
    Client* client = findClientByFd(client_fd);
    if (!client) {
        return;
    }
    
    if (!client->isRegistered()) {
        std::string error = RPL::ERR_NOTREGISTERED(getServerName(), client->getNickName());
        client->sendMessage(error);
        return;
    }
    
    std::string nick = client->getNickName();
    
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) { 
        std::ostringstream oss;
        oss << it->second.getClientCount();
        std::string listReply = RPL::RPL_LIST(getServerName(), nick, it->first, 
            oss.str(), it->second.getTopic());
        client->sendMessage(listReply);
    }
    std::string endMsg = RPL::RPL_LISTEND(getServerName(), nick);
    client->sendMessage(endMsg);
}