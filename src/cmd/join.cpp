/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 12:15:43 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/18 12:19:37 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::JoinCommand(int client_fd, const std::vector<std::string> &tokens)
{
	if (tokens.size() < 2)
	{
		std::cout << "JOIN: insufficient parameters" << std::endl;
		return;
	}

	std::string channel = tokens[1];

	std::cout << "Client" << client_fd << "wants to join the channel: " << channel << std::endl;
}