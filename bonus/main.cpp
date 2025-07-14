/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 21:01:20 by albelope          #+#    #+#             */
/*   Updated: 2025/07/14 19:02:46 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Bot.hpp"
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <arpa/inet.h>

int	valid_ip(const std::string& ip)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));
	if (result == 1)
		return (0);	// Valid IP
	return (1);		// Invalid IP
}

int	valid_password(std::string password)
{
	if (password.empty() || password.length() < 4 || password.length() > 20 ||
		password[0] == ' ' || password[password.length() -1] == ' ')
		return (1);
	return (0);
}

int	valid_port(char *str)
{
	int i = 0;

	while (str[i])
	{
		if (isdigit(str[i]))
			i++;
		else
			return (1);
	}
	if (std::atoi(str) < 1024 || std::atoi(str) > 65535)
		return (1);
	return (0);
}

int main(int argc, char **argv)
{
	if (argc != 5)
	{
		std::cerr << "Usage: ./bonus <ip> <port> <password> <channel>" << std::endl;
		return (1);
	}

	if (valid_ip(argv[1]) == 1)
	{
		std::cerr << "Error: Invalid IP address" << std::endl;
		return (1);
	}

	if (valid_port(argv[2]) == 1)
	{
		std::cerr << "Error: Invalid port. Please enter a number between 1024 and 65535" << std::endl;
		return (1);
	}

	if (valid_password(argv[3]) == 1)
	{
		std::cerr << "Error: Invalid password" << std::endl;
		return (1);
	}

	std::string ip = argv[1];
	int port = std::atoi(argv[2]);
	std::string password = argv[3];
	std::string channel = argv[4];

	Bot bot(ip, port, password, channel);
	bot.start();

	return (0);
}