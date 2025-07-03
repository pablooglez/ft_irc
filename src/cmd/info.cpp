/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   info.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 20:27:12 by albelope          #+#    #+#             */
/*   Updated: 2025/07/03 11:06:38 by albelope         ###   ########.fr       */
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
        std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), getClientNick(client_fd), "INFO");
        client->sendMessage(error);
        return ;
    }
 
    if (!client->isRegistered()) {
        std::string error = RPL::ERR_NOTREGISTERED(getServerName(), getClientNick(client_fd));
        client->sendMessage(error);
        return;
    }

    std::string nickName = client->getNickName();
    std::string serverName = getServerName();


std::string theNickName = client->getNickName();
std::string theserverName   = getServerName();


client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "==================================================================="));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "|                           FT_IRC SERVER                           |"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "|                        42 Telefonica Project                       |"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "==================================================================="));

std::stringstream port_ss;
port_ss << GetPort();
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Server Port      : " + port_ss.str()));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Server Created   : June 28, 2025"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Your Host        : " + client->getHostName()));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  IRC Version      : 1.0"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  IRC Protocol     : RFC 2812 (Core)"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Supported Modes  :"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "      +i  invite-only"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "      +t  topic restricted"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "      +k  password"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "      +l  user limit"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Max Clients      : 200")); // SERIA BUENA IDEA PONER UN MAX DE CLIENTES
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "-------------------------------------------------------------------"));

std::stringstream users_ss;                                                                            
users_ss << _clients.size();
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Connected Users  : " + users_ss.str()));

std::stringstream channels_ss;
channels_ss << _channels.size();
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Active Channels  : " + channels_ss.str()));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Supported Commands:"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "      PASS   |   NICK   |   USER   |   JOIN  |   KICK    | INVITE"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "      PRIVMSG|   QUIT   |   LIST   |   NAMES |   TOPIC"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Project Authors:"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "      @Pablogon  - Pablo Gonzalez"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "      @Albelope  - A. Israel Alcaide"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Welcome " + nickName + ", enjoy our FT_IRC project from MALAGA."));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Sun Code and beach with espetos and Victoria cold beer."));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "  Remember: Suffer the code is Art. Welcome to  our  42 FT_IRC project."));
client->sendMessage(RPL::RPL_INFO(theserverName, theNickName, "==================================================================="));
client->sendMessage(RPL::RPL_ENDOFINFO(theserverName, theNickName, "End of INFO list"));





    

    
    
    
    
    
}