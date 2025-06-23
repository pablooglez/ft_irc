/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 13:01:59 by albelope          #+#    #+#             */
/*   Updated: 2025/06/23 13:25:26 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::PrivmsgCommand(int client_fd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 3)
		return ;
	std::string command = tokens[0];
	std::string target = tokens[1];
	std::string message = tokens[2];
}