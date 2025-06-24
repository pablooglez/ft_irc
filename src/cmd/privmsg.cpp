/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:01:59 by albelope          #+#    #+#             */
/*   Updated: 2025/06/24 14:14:37 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
#include "../../include/Channel.hpp"

void	Server::PrivmsgCommand(int client_fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 3)
		return ;
	std::string command = tokens[0];
	std::string target = tokens[1];
	std::string message = tokens[2];
	//ordenamos los string en base al orden de mensajes.

	Client *senderId = findClientByFd(client_fd);		// Obtenemos el puntero al objeto Client que envia el mensaje usando su file descriptor
														// Necesario porque client_fd es just a numero, necesitamos el objeto completo para operaciones
	if (!senderId)
		return ;
	if (target[0] == '#') {
		Channel* canal = findChannelByName(target);		// Busca el canal en la coleccion _channels del servidor usando el nombre completo
		if (!canal)
			return ;
		if (!canal->isMember(senderId))
			return ; 									// Verificamos el sender este en la lista _members del canal
		canal->broadcastToOthers(message, senderId);	// Enviamos el mensaje a todos los _members del canal EXCEPTO al sender
	}
	else {
		Client* user = findClientByNickname(target);	// Busca el usuario destino en la colección _clients del servidor por nickname
		if (!user)
			return ;
		user->sendMessage(message);
	}
	//	implemENTAR LOS MENSAJES DE ERROR CORRESPONDIENTES EN RAWLOG
}