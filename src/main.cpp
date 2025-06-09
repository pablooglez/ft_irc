/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 18:37:42 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/09 18:16:42 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

int	g_global = 0; // Global variable

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

int	main(int argc, char **argv)
{
	(void)argv;

	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserver <port> <password>" << std::endl;
		return (1);
	}

	if (valid_port(argv[1]) == 1)
	{
		std::cerr << "Error: Invalid port. Please enter a number between 1024 and 65535" << std::endl;
		return (1);
	}

	if (valid_password(argv[2]) == 1)
	{
		std::cerr << "Error: Invalid password" << std::endl;
		return (1);
	}

	Server server(std::atoi(argv[1]), argv[2]);
	server.start();

	return (0);
}