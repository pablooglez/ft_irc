/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 18:18:05 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/16 18:37:36 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::QuitCommand(int client_fd, const std::vector<std::string> &tokens)
{
	std::string quit_message = "Client quit";

	if (tokens.size() > 1)
		quit_message = tokens[1];

	std::cout << "Client " << client_fd << " is disconnected: " << quit_message << std::endl;
	removeClient(client_fd);
}