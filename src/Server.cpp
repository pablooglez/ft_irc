/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:05:24 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/06 19:40:05 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(int port, const std::string &password)
{
	this->_port = port;
	this->_password = password;
	this->_server_fd = -1;
	this->_running = false;
}

Server::~Server()
{
	cleanup();
}

int	Server::GetPort() const
{
	return this->_port;
}

const std::string	&Server::GetPassword() const
{
	return this->_password;
}

int	Server::getServerFd() const
{
	return this->_server_fd;
}

bool	Server::setupSocket()
{
	std::cout << "....Configure setupSocket...." << std::endl;

	this->_server_fd = socket(AF_INET, SOCK_STREAM, 0); // Create Socket

	if (this->_server_fd == -1)
	{
		std::cerr << "Error: socket() failed" << std::endl;
		return (false);
	}

	int	opt = 1;
	if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // Configure SO_REUSEADDR
	{
		std::cerr << "Error: setsockopt() failed" << std::endl;
		return (false);
	}

	std::memset(&this->_server_addr, 0, sizeof(this->_server_addr)); // Configure Server Address
	this->_server_addr.sin_family = AF_INET; // IPV4
	this->_server_addr.sin_addr.s_addr = INADDR_ANY; // ALL INTERFACES (0.0.0.0)
	this->_server_addr.sin_port = htons(this->_port); // Network byte order

	if (bind(this->_server_fd, (struct sockaddr*) &this->_server_addr, sizeof(this->_server_addr)) < 0) // Bind - "Reserve" the port
	{
		std::cerr << "Error: bind() failed" << std::endl;
		cleanup();
		return (false);
	}

	if (listen(this->_server_fd, SOMAXCONN) < 0) // Start listening for connections
	{
		std::cerr << "Error: listen() failed" << std::endl;
		cleanup();
		return (false);
	}

	if (fcntl(this->_server_fd, F_SETFL, O_NONBLOCK)) // Socket Not-blocking
	{
		std::cerr << "Error: fcntl() failed" << std::endl;
		cleanup();
		return (false);
	}

	std::cout << "Server socket created succesfully" << std::endl;
	std::cout << "Listening in port: " << _port << std::endl;
	std::cout << "Password: " << _password << std::endl;

	return (true);
}

void	Server::start()
{
	if (!setupSocket())
	{
		std::cerr << "Error: Failed to setup server socket" << std::endl;
		return;
	}

	this->_running = true;

	std::cout << "    SERVER STARTED!!!!    " << std::endl;

	while (this->_running)
	{
		sleep(1);
	}
}

void	Server::stop()
{
	this->_running = false;

	cleanup();
}

void	Server::cleanup()
{
	if (this->_server_fd != -1)
	{
		close(this->_server_fd);
		this->_server_fd = -1;
	}
}