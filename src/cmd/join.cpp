/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 12:15:43 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/21 18:24:31 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::JoinCommand(int client_fd, const std::vector<std::string> &tokens)
{
	Client *client = findClientByFd(client_fd);
	
	if (!client)
	{
		std::cerr << "Error: Client not found for fd " << client_fd << std::endl;
		return;
	}

	if (!client->isRegistered())
	{
		std::string error = ":localhost 451 " + client->getNickName() + " :You have not registered\r\n";
		client->sendMessage(error);
		return;
	}

	if (tokens.size() < 2)
	{
		std::string error = ":localhost 461 " + client->getNickName() + " JOIN :Not enough parameters\r\n";
		client->sendMessage(error);
		return;
	}

	std::string channel_name = tokens[1];
	
	// Ensure channel name starts with #
	if (channel_name[0] != '#')
	{
		channel_name = "#" + channel_name;
	}

	// For now, just send a basic response that the user joined the channel
	std::string join_response = ":" + client->getNickName() + "!" + client->getUserName() + "@" + client->getHostName() + " JOIN " + channel_name + "\r\n";
	client->sendMessage(join_response);

	// Send topic (empty for now)
	std::string topic_response = ":localhost 331 " + client->getNickName() + " " + channel_name + " :No topic is set\r\n";
	client->sendMessage(topic_response);

	// Send names list (just the user for now)
	std::string names_response = ":localhost 353 " + client->getNickName() + " = " + channel_name + " :" + client->getNickName() + "\r\n";
	client->sendMessage(names_response);

	// End of names list
	std::string end_names = ":localhost 366 " + client->getNickName() + " " + channel_name + " :End of /NAMES list\r\n";
	client->sendMessage(end_names);

	std::cout << "Client " << client->getNickName() << " joined channel " << channel_name << std::endl;
}