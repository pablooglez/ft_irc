/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 17:46:10 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/16 17:53:56 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::vector<std::string> Server::splitMessage(const std::string &message, char delimiter)
{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = message.find(delimiter);

	while (end != std::string::npos)
	{
		if (start != end)
			tokens.push_back(message.substr(start, end - start));
		start = end + 1;
		end = message.find(delimiter, start);
	}
	if (start < message.size())
		tokens.push_back(message.substr(start));
	return tokens;
}