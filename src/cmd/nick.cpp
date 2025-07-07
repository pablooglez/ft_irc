/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 16:45:52 by pablogon          #+#    #+#             */
<<<<<<< HEAD
/*   Updated: 2025/06/27 20:53:27 by pablogon         ###   ########.fr       */
=======
/*   Updated: 2025/07/02 17:36:40 by albelope         ###   ########.fr       */
>>>>>>> Basura-Channel
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::NickCommand(int client_fd, const std::vector<std::string> &tokens)
{
	Client *client = findClientByFd(client_fd);	// Search a Client

	if (!client)
	{
		std::cerr << "Error: Client not found for fd" << client_fd << std::endl;
		return;
	}

	if (tokens.size() < 2)	// Check that a nickname is provided
	{
		std::string error = RPL::ERR_NONICKNAMEGIVEN(getServerName(), getClientNick(client_fd));
		client->sendMessage(error);
		return;
	}

	if (!client->isAuthenticated())	// Verify that the client is authenticated (PASS)
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), getClientNick(client_fd));
		client->sendMessage(error);
		return;
	}

	std::string new_nickname = tokens[1];

	if (!isValidNickname(new_nickname))		// Validate nickname format
	{
		std::string error = RPL::ERR_ERRONEUSNICKNAME(getServerName(), getClientNick(client_fd), new_nickname);
		client->sendMessage(error);
		return;
	}

	if (isNicknameInUse(new_nickname))	// Check that the nickname is not in use
	{
		std::string error = RPL::ERR_NICKNAMEINUSE(getServerName(), getClientNick(client_fd), new_nickname);
		client->sendMessage(error);
		return;
	}

	bool is_nick_change = client->hasNickName();
	std::string old_nickname;

	if (is_nick_change)
	{
		old_nickname = client->getNickName();

		if (old_nickname == new_nickname)	// If try to change the same nickname, ignore
			return;
	}

	client->setNickName(new_nickname);	// Update client nickname
	client->setHasNickName(true);

	if (is_nick_change)	// If a nickname change, notify other users
	{
		std::string nick_change_msg = ":" + old_nickname + "!" + client->getUserName() + "@" + client->getHostName() + " NICK :" + new_nickname + "\r\n";

		client->sendMessage(nick_change_msg);	// Send to your own client

		std::vector<int> notified_clients;	// Send to all users who share channels

		const std::vector<Channel*> &channels = client->getChannels();
		for (std::vector<Channel*>::const_iterator it = channels.begin(); it != channels.end(); ++it)
		{
			Channel *channel = *it;
<<<<<<< HEAD
			const std::vector<Client*> &channel_clients = channel->getClients();
=======
			const std::vector<Client*> &channel_clients = channel->getMembers();
>>>>>>> Basura-Channel
			for (std::vector<Client*>::const_iterator client_it = channel_clients.begin(); client_it != channel_clients.end(); ++client_it)
			{
				Client *channel_client = *client_it;
				if (channel_client->getFd() != client_fd && std::find(notified_clients.begin(), notified_clients.end(), channel_client->getFd()) == notified_clients.end())
				{
					channel_client->sendMessage(nick_change_msg);
					notified_clients.push_back(channel_client->getFd());
				}
			}
		}
	}

	client->updateRegistrationStatus();	// Update registration status

	std::cout << "Client " << client_fd << " changed nickname to: " << new_nickname << std::endl;	// Success log

	if (client->isRegistered())
	{
		std::cout << "Client " << client_fd << " is now fully registered!" << std::endl;
	}
}