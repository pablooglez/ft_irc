/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   info.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 20:27:12 by albelope          #+#    #+#             */
/*   Updated: 2025/07/03 18:06:22 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
#include <sstream>


void Server::InfoCommand(int client_fd, std::vector<std::string> &tokens) {
    
    Client* client = findClientByFd(client_fd);
    
    if (!client) {
        std::cerr << "Error: Client not found " << client_fd << std::endl;
        return;
    }
    
    if (tokens.size() > 1) {
        std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "INFO");
        client->sendMessage(error);
        return;
    }
 
    if (!client->isRegistered()) {
        std::string error = RPL::ERR_NOTREGISTERED(getServerName(), client->getNickName());
        client->sendMessage(error);
        return;
    }

    std::string nickName = client->getNickName();
    std::string serverName = getServerName();

client->sendMessage(RPL::RPL_INFO(serverName, nickName, "==================================================================="));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "|                           FT_IRC SERVER                          |"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "|                        42 Telefonica Project                     |"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "==================================================================="));

std::stringstream port_ss;
port_ss << GetPort();
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Server Port      : " + port_ss.str()));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Server Created   : June 28, 2025"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Your Host        : " + client->getHostName()));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  IRC Version      : 1.0"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  IRC Protocol     : RFC 2812 (Core)"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Supported Modes  :"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "      +i  invite-only      |   +t  topic restricted"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "      +k  password         |   +l  user limit"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Max Clients      : 200")); // SERIA BUENA IDEA PONER UN MAX DE CLIENTES
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "-------------------------------------------------------------------"));

std::stringstream users_ss;                                                                            
users_ss << _clients.size();
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Connected Users  : " + users_ss.str()));

std::stringstream channels_ss;
channels_ss << _channels.size();
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Active Channels  : " + channels_ss.str()));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Supported Commands:"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "      PASS   |   NICK   |   USER   |   JOIN  |   KICK    | INVITE"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "      PRIVMSG|   QUIT   |   LIST   |   NAMES |   TOPIC"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Project Authors:"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "      @Pablogon  - Pablo Gonzalez"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "      @Albelope  - A. Israel Alcaide"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Welcome " + nickName + ", enjoy our FT_IRC project from MALAGA."));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Sun Code and beach with espetos and Victoria cold beer."));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "  Remember: Suffer the code is Art. Welcome to  our  42 FT_IRC project."));
client->sendMessage(RPL::RPL_INFO(serverName, nickName, "==================================================================="));
client->sendMessage(RPL::RPL_ENDOFINFO(serverName, nickName, "End of INFO list"));





    

    
    
    
    
    
}