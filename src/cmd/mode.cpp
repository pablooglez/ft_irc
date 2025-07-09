/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:02:04 by albelope          #+#    #+#             */
/*   Updated: 2025/07/08 20:50:42 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::handleInvideMode(Channel *channel, Client *client, bool checkMode)
{
	if (checkMode == true)
		channel->setInviteOnly(true);
	else
		channel->setInviteOnly(false);

	std::string modeInvMessage;
	modeInvMessage = ":" + client->getNickName() + " MODE " + channel->getChannelName() + " ";

	if (checkMode)
		modeInvMessage = modeInvMessage + "+i";
	else
		modeInvMessage = modeInvMessage + "-i";

	channel->broadcast(modeInvMessage, NULL);
	client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(), channel->getChannelName(), channel->getModes()));
}

void	Server::handleTopicMode(Channel *channel, Client *client, bool checkMode)
{
	if (checkMode == true)
		channel->setTopicRestricted(true);
	else	
		channel->setTopicRestricted(false);

	std::string modeTopicMessage;
	modeTopicMessage = ":" + client->getNickName() + " MODE " + channel->getChannelName() + " ";

	if (checkMode)
		modeTopicMessage = modeTopicMessage + "+t";
	else
		modeTopicMessage = modeTopicMessage + "-t";

	channel->broadcast(modeTopicMessage, NULL);
	client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(), channel->getChannelName(), channel->getModes()));
}

void	Server::handleLimitMode(Channel *channel, Client *client, bool checkMode, const std::string& parameter)
{
	int newLimit = std::atoi(parameter.c_str());
	
	if (checkMode == true)
	{
		if (newLimit <= 0)
			return ;
		channel->setUserLimit(newLimit);
	}
	else
		channel->setUserLimit(0);
		
	std::string modeLimitMessage;
	modeLimitMessage = ":" + client->getNickName() + " MODE " + channel->getChannelName() + " ";

	if (checkMode)
	{
		modeLimitMessage = modeLimitMessage + "+l " + parameter;
	}
	else
	{
		modeLimitMessage = modeLimitMessage + "-l";
	}

	channel->broadcast(modeLimitMessage, NULL); 
	client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(), channel->getChannelName(), channel->getModes()));
}



void	Server::handleKeyMode(Channel *channel, Client *client, bool checkMode, const std::string& parameter)
{
	std::string newPassword = parameter;

	if (checkMode == true)
	{
		channel->setPassword(newPassword);
	}
	else
	{
		channel->setPassword("");
	}

	std::string modeKeyMessage;
	modeKeyMessage = ":" + client->getNickName() + " MODE " + channel->getChannelName() + " ";
	
	if (checkMode)
	{
		modeKeyMessage = modeKeyMessage + "+k " + parameter;
	}
	else
	{
		modeKeyMessage = modeKeyMessage + "-k";
	}

	channel->broadcast(modeKeyMessage, NULL);
	client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(), channel->getChannelName(), channel->getModes()));
}

void	Server::handleUnknownMode(Channel *channel, Client *client, bool checkMode)
{
	(void)channel;
	(void)checkMode;

	client->sendMessage(RPL::ERR_UNKNOWNMODE(getServerName(), client->getNickName(), "unknown"));
}

void	Server::handleOperatorMode(Channel *channel, Client *client, bool checkMode, const std::string &parameter)
{
	Client *targetClient = findClientByNickname(parameter);	// Search for target customer by nickname

	if (!targetClient)	// Verify if Client exists
	{
		std::string error = RPL::ERR_NOSUCHNICK(getServerName(), client->getNickName(), parameter);
		client->sendMessage(error);
		return;
	}

	if (!channel->isMember(targetClient))	// Verify if Client is in the channel
	{
		std::string error = RPL::ERR_USERNOTINCHANNEL(getServerName(), client->getNickName(), parameter, channel->getChannelName());
		client->sendMessage(error);
		return;
	}

	std::string modeOpMessage = ":" + client->getNickName() + " MODE " + channel->getChannelName() + " ";

	if (checkMode)	// +o
	{
		if (channel->addOperator(targetClient))
			modeOpMessage += "+o " + parameter;
		else
			return;
	}
	else	// -o
	{
		if (channel->removeOperator(targetClient))
			modeOpMessage += "-o " + parameter;
		else
			return;
	}
	channel->broadcast(modeOpMessage, NULL);
}

void	Server::ModesCommand(int client_fd, std::vector<std::string> &tokens)
{
	
	Client* client = findClientByFd(client_fd);
	
	if (!client)
	{
		return;
	}

	if (tokens.size() < 2)
	{
		std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "MODE");

		client->sendMessage(error);
		return;
	}
	
	std::string channelName = tokens[1];

	if (!client->isRegistered())
	{
		std::string error = RPL::ERR_NOTREGISTERED(getServerName(), client->getNickName());
		client->sendMessage(error);
		return;
	}

	Channel* channel = findChannel(channelName);

	if (!channelExists(channelName))
	{
		std::string error = RPL::ERR_NOSUCHCHANNEL(getServerName(), client->getNickName(), channelName);
		client->sendMessage(error);
		return;
	}

	if (tokens.size() == 2)
	{
		client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(), channel->getChannelName(), channel->getModes()));
		return ;
	}


	if (tokens.size() >= 3)
	{
		
		std::string modes = tokens[2];
		
		if (!channel->isOperator(client)) 
		{
			std::string error = RPL::ERR_CHANOPRIVSNEEDED(getServerName(), client->getNickName(), channelName);
			client->sendMessage(error);
			return;
		}
		
		size_t argIndex = 3;

		if (modes[0] != '+' && modes[0] != '-')
		{
			std::string error = RPL::ERR_UNKNOWNMODE(getServerName(), client->getNickName(), std::string(1, modes[0]));
			client->sendMessage(error);
			return;
		}

		if (modes.length() == 1)
		{
			std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "MODE");
			client->sendMessage(error);
			return;
		}

		bool checkMode = true;
		for (size_t i = 0; i < modes.length(); i++)
		{
			char modeChar = modes[i];
			
			if (modeChar == '+') 
			{
				checkMode = true;
				continue ;
			}
			
			if (modeChar == '-')
			{
				checkMode = false;
				continue ;
			}
			
			switch (modeChar)
			{
				case 'i':
					handleInvideMode(channel, client, checkMode);
					break;
				
				case 'l':
					if (checkMode)
					{
						if (tokens.size() <= argIndex)
						{
							std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "MODE");
							client->sendMessage(error);
							return;
						}
						handleLimitMode(channel, client, checkMode, tokens[argIndex]);
						argIndex++;
					}
					else
					{
						handleLimitMode(channel, client, checkMode, "");
					}
					break;
					
				case 't':
					handleTopicMode(channel, client, checkMode);
					break;
				
				case 'k':
					if (checkMode)
					{
						if (tokens.size() <= argIndex)
						{
							std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "MODE");
							client->sendMessage(error);
							return;
						}
						handleKeyMode(channel, client, checkMode, tokens[argIndex]);
						argIndex++;
					}
					else
					{
						handleKeyMode(channel, client, checkMode, "");
					}
					break;

				case 'o':
					if (tokens.size() <= argIndex)
					{
						std::string error = RPL::ERR_NEEDMOREPARAMS(getServerName(), client->getNickName(), "MODE");
						client->sendMessage(error);
						return;
					}
					handleOperatorMode(channel, client, checkMode, tokens[argIndex]);
					argIndex++;
					break;
				
				default:
					handleUnknownMode(channel, client, checkMode);
					return; 
				}
			}
		}
	}


