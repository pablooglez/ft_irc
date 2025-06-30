/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:02:06 by albelope          #+#    #+#             */
/*   Updated: 2025/06/30 13:34:49 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::ListCommand(int client_fd, std::vector<std::string> &tokens)
{
    std::string nick = getClientNick(client_fd);
    std::string server = getServerName();
    

    std::string response = ":" + server + " 321 " + nick + " Channel :Users Name\r\n";
    send(client_fd, response.c_str(), response.length(), 0);
    

	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
    	std::string listReply = RPL::RPL_LIST(getServerName(), nick, it->first, 
        	std::to_string(it->second.getClientCount()), it->second.getTopic());
    	send(client_fd, listReply.c_str(), listReply.length(), 0);
	}
	std::string endMsg = RPL::RPL_LISTEND(getServerName(), nick);
	send(client_fd, endMsg.c_str(), endMsg.length(), 0);
}