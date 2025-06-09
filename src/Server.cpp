/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:05:24 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/09 20:40:13 by pablogon         ###   ########.fr       */
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

	if (fcntl(this->_server_fd, F_SETFL, O_NONBLOCK) == -1) // Socket Not-blocking
	{
		std::cerr << "Error: fcntl() failed" << std::endl;
		cleanup();
		return (false);
	}

	// Configure poll for server socket
	struct pollfd server_pollfd;
	server_pollfd.fd = this->_server_fd;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;

	this->_poll_fds.push_back(server_pollfd);	// Add server socket to poll arraya

	
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

	std::cout << "    SERVER STARTED    " << std::endl;

	runServerLoop();
}

void	Server::stop()
{
	this->_running = false;

	cleanup();
}

void	Server::cleanup()
{
	// Cerrar todos los sockets de clientes
	for (size_t i = 0; i < this->_client_fds.size(); i++)
	{
		close(this->_client_fds[i]);
	}
	this->_client_fds.clear();
	
	// Limpiar array de poll
	this->_poll_fds.clear();
	
	// Cerrar socket del servidor
	if (this->_server_fd != -1)
	{
		close(this->_server_fd);
		this->_server_fd = -1;
	}
	
	std::cout << "Server cleanup completed" << std::endl;
}

void	Server::handleSignal(int signal)
{
	g_global = signal;
}

void	Server::runServerLoop()
{
	signal(SIGINT, Server::handleSignal);

	while (this->_running)
	{
		if (g_global == SIGINT)
		{
			this->_running = false;
			break;
		}

		int	poll_result = poll(&this->_poll_fds[0], this->_poll_fds.size(), 1000);

		if (poll_result == -1 && g_global == 0)
		{
			std::cerr << "Error: poll() failed" << std::endl;
			break;
		}
		if (poll_result == 0)
		{
			continue;
		}

		for (size_t i = 0; i < this->_poll_fds.size(); i++)
		{
			if (this->_poll_fds[i].revents & POLLIN)
			{
				if (this->_poll_fds[i].fd == this->_server_fd)
					handleNewConnection();
				else
					handleClientData(this->_poll_fds[i].fd);
			}
			if (this->_poll_fds[i].revents & (POLLHUP | POLLERR))
			{
				// Handle client disconnection
				if (this->_poll_fds[i].fd != this->_server_fd)
				{
					std::cout << "Error: Client disconnected (error/hangup) (fd: " << this->_poll_fds[i].fd << ")" << std::endl;
					removeClient(this->_poll_fds[i].fd);
					i--;	// Adjust index because we removed an element
				}
			}
		}
	}
	std::cout << "....SERVER STOPPED...." << std::endl;
}

void Server::handleNewConnection()
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	
	int client_fd = accept(this->_server_fd, (struct sockaddr*)&client_addr, &client_len); // Accept new connection
	if (client_fd == -1)
	{
		std::cerr << "Error: accept() failed" << std::endl;
		return;
	}

	// Configurar socket cliente como no-bloqueante
	int flags = fcntl(client_fd, F_GETFL, 0);
	if (flags == -1 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Error: fcntl() failed for client" << std::endl;
		close(client_fd);
		return;
	}

	// Obtener IP del cliente para logging
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
	
	std::cout << "New connection from " << client_ip << ":" << ntohs(client_addr.sin_port) << " (fd: " << client_fd << ")" << std::endl;

	// Crear entrada para poll
	struct pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;  // Solo datos entrantes por ahora
	client_pollfd.revents = 0;

	// Añadir a los arrays
	this->_poll_fds.push_back(client_pollfd);
	this->_client_fds.push_back(client_fd);
	
	std::cout << "Client added. Total clients: " << this->_client_fds.size() << std::endl;
	
	// Mensaje inicial (solicitar password)
	std::string welcome_msg = "Enter the password (PASS <password>)\r\n";
	send(client_fd, welcome_msg.c_str(), welcome_msg.length(), 0);
}