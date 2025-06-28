/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   info.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 20:27:12 by albelope          #+#    #+#             */
/*   Updated: 2025/06/28 22:12:25 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"


void Server::commandInfo(int client_fd, std::vector<std::string> &tokens) {
    
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
    std::string server = getServerName();


std::string nickName = client->getNickName();
std::string server   = getServerName();


client->sendMessage(RPL::RPL_INFO(server, nickName, "==================================================================="));
client->sendMessage(RPL::RPL_INFO(server, nickName, "|                           FT_IRC SERVER                           |"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "|                        42 Telefonica Project                       |"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "==================================================================="));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Server Name      : " + server));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Server Port      : " + std::to_string(GetPort())));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Server Created   : June 28, 2025"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Your Host        : " + client->getHostName()));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  IRC Version      : 1.0"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  IRC Protocol     : RFC 2812 (Core)"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Supported Modes  :"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "      +i  invite-only"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "      +t  topic restricted"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "      +k  password"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "      +l  user limit"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Max Clients      : 200")); // SERIA BUENA IDEA PONER UN MAX DE CLIENTES
client->sendMessage(RPL::RPL_INFO(server, nickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Connected Users  : " + std::to_string(_clients.size())));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Active Channels  : " + std::to_string(_channels.size())));
client->sendMessage(RPL::RPL_INFO(server, nickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Supported Commands:"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "      PASS   |   NICK   |   USER   |   JOIN"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "      PRIVMSG|   QUIT   |   LIST   |   NAMES"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "      KICK   |   TOPIC  |   INVITE"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Project Authors:"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "      @Pablogon  - Pablo Gonzalez"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "      @Albelope  - A. Israel Alcaide"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "-------------------------------------------------------------------"));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Welcome " + nickName + ", enjoy our FT_IRC project from MALAGA."));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Sun Code and beach with espetos and Victoria cold beer."));
client->sendMessage(RPL::RPL_INFO(server, nickName, "  Remember: Suffer the code is Art. Welcome to  our  42 FT_IRC project."));
client->sendMessage(RPL::RPL_INFO(server, nickName, "==================================================================="));
client->sendMessage(RPL::RPL_ENDOFINFO(server, nickName, "End of INFO list"));





    

    
    
    
    
    
}