/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:02:04 by albelope          #+#    #+#             */
/*   Updated: 2025/07/03 11:57:29 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"


void	Server::handleInvideMode(Channel *channel, Client *client, bool checkMode) {

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
	client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(),
									channel->getChannelName(), channel->getModes()));
}


void	Server::handleTopicMode(Channel *channel, Client *client, bool checkMode) {
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
	client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(),
									channel->getChannelName(), channel->getModes()));
}


void	Server::handleLimitMode(Channel *channel, Client *client, bool checkMode,
						const std::string& parameter) {
	int newLimit = std::atoi(parameter.c_str());
	if (checkMode == true) {
		if (newLimit <= 0)
			return ;
		channel->setUserLimit(newLimit);
	}
	else
		channel->setUserLimit(0);
		
    std::string modeLimitMessage;
    modeLimitMessage = ":" + client->getNickName() + " MODE " + channel->getChannelName() + " ";
    if (checkMode) {
        modeLimitMessage = modeLimitMessage + "+l " + parameter;
    } else {
        modeLimitMessage = modeLimitMessage + "-l";
    }
    
    channel->broadcast(modeLimitMessage, NULL); 
    client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(),
                                    channel->getChannelName(), channel->getModes()));
	
}



void	Server::handleKeyMode(Channel *channel, Client *client, bool checkMode,
						const std::string& parameter) {
	std::string newPassword = parameter;
	if (checkMode == true) {
		channel->setPassword(newPassword);
	}
	else {
		channel->setPassword("");
	}

	std::string modeKeyMessage;
	modeKeyMessage = ":" + client->getNickName() + " MODE " + channel->getChannelName() + " ";
	
	if (checkMode) {
		modeKeyMessage = modeKeyMessage + "+k " + parameter;
	}
	else {
		modeKeyMessage = modeKeyMessage + "-k";
	}

	channel->broadcast(modeKeyMessage, NULL); 
    client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(),
                                    channel->getChannelName(), channel->getModes()));
}



void	Server::handleUnknownMode(Channel *channel, Client *client, bool checkMode) {
	
	(void)channel;
	(void)checkMode;
	client->sendMessage(RPL::ERR_UNKNOWNMODE(getServerName(), client->getNickName(), "unknown"));
}
	

void	Server::ModesCommand(int client_fd, std::vector<std::string> &tokens) {
	
	if (tokens.size() < 2) {
		std::cout << "Error : parameteres" << std::endl;
		return ;
	}
	std::string channelName = tokens[1];
	
	
	Client* client = findClientByFd(client_fd);
	
	if (!client){
		std::cout << "Error : client not found" << std::endl;
		return ;
	}

	if (!client->isRegistered()) {
		std::cout << "Error: client not registered" << std::endl;
		return ;
	}

	Channel* channel = findChannelByName(channelName);

	if (!channelExists(channelName)) {
		std::cout << "Error : channel not found" << std::endl;
		return ;
	}

	if (tokens.size() == 2) {
		client->sendMessage(RPL::RPL_CHANNELMODEIS(getServerName(), client->getNickName(),
						channel->getChannelName(), channel->getModes()));
		return ;
	}


	if (tokens.size() >= 3) {
		
		std::string modes = tokens[2];
		
		if (!channel->isOperator(client)) {
			std::cout << "RPL ERR_CHANOPRIVVSNEEDE" << std::endl;
			return ;
		}
		
		size_t argIndex = 3;

		if (modes[0] != '+' && modes[0] != '-') {
			std::cout << "RPL HERE" << std::endl;
			return ;
		}
		if (modes.length() == 1) {
    		std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    		return ;
		}

		bool checkMode = true;
		for (size_t i = 0; i < modes.length(); i++) {
			char modeChar = modes[i];
			
			if (modeChar == '+') {
				checkMode = true;
				continue ;
			}
			
			if (modeChar == '-') {
				checkMode = false;
				continue ;
			}
			
			switch (modeChar) {
				case 'i':
					handleInvideMode(channel, client, checkMode);
					break;
				
				case 'l':
					if (checkMode) {
						if (tokens.size() <= argIndex) {
							std::cout << "ERROR RPL" << std::endl;
							return;
						}
						handleLimitMode(channel, client, checkMode, tokens[argIndex]);
						argIndex++;
					} else {
						handleLimitMode(channel, client, checkMode, "");
					}
					break;
					
				case 't':
					handleTopicMode(channel, client, checkMode);
					break;
				
				case 'k':
					if (checkMode) {
						if (tokens.size() <= argIndex) {
							std::cout << "ERROR RPL" << std::endl;
							return;
						}
						handleKeyMode(channel, client, checkMode, tokens[argIndex]);
						argIndex++;
					} else {
						handleKeyMode(channel, client, checkMode, "");
					}
					break;
				
				default:
					handleUnknownMode(channel, client, checkMode);
					return; 
				}
			}
		}
	}


