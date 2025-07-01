/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:02:04 by albelope          #+#    #+#             */
/*   Updated: 2025/07/01 13:45:07 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"


void	Server::handleInvideMode(Channel *channel, Client *client, bool checkMode) {
	
}
void	Server::handleTopicMode(Channel *channel, Client *client, bool checkMode) {
	
}
void	Server::handleLimitMode(Channel *channel, Client *client, bool checkMode,
						const std::string& parameter) {
	
}
void	Server::handleKeyMode(Channel *channel, Client *client, bool checkMode,
						const std::string& parameter) {
	
}
void	Server::handleUnknownMode(Channel *channel, Client *client, bool checkMode) {
	
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
						handleUknownMode(channel, client, checkMode);
						return;
				}
			}
		}
	}







/*[X] Validaciones básicas
  [X] Consulta de modos
  [ ] (Opcional) RPL_CREATIONTIME
  [ ] Comprobar si el cliente es operador
  [ ] Leer y analizar el string de modos
  [ ] Verificar modos válidos
  [ ] Verificar parámetros necesarios
  [ ] Aplicar los cambios
  [ ] Hacer broadcast al canal
  [ ] Implementar los errores necesarios*/
