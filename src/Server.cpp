/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:05:24 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/18 12:14:34 by pablogon         ###   ########.fr       */
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

	this->_poll_fds.push_back(server_pollfd);	// Add server socket to poll array

	
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
	for (size_t i = 0; i < this->_client_fds.size(); i++) // Close all client sockets
	{
		close(this->_client_fds[i]);
	}
	this->_client_fds.clear();

	this->_poll_fds.clear(); // Clean poll array

	if (this->_server_fd != -1) // Close server socket
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

		int	poll_result = poll(&this->_poll_fds[0], this->_poll_fds.size(), -1);

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
					std::cout << "Error: Client disconnected (error/hangup)" << std::endl;
					removeClient(this->_poll_fds[i].fd);
					i--;	// Adjust index because we removed an element
				}
			}
		}
	}
	std::cout << "    SERVER STOPPED    " << std::endl;
}

void	Server::handleNewConnection()
{
	std::cout << "...Handling new connection..." << std::endl;

	struct sockaddr_in client_addr;	// Structure for handling customer information
	socklen_t client_len = sizeof(client_addr);

	//Accept() accept pending connection
	int	client_fd = accept(this->_server_fd, (struct sockaddr*) &client_addr, &client_len);
	
	if (client_fd == -1)
	{
		std::cerr << "Error: accept() failed" << std::endl;
		return;
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) // Socket Not-blocking
	{
		std::cerr << "Error: fcntl() failed for client socket" << std::endl;
		close(client_fd);
		return;
	}
	
	// Add the client to the poll array
	struct pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;
	
	this->_poll_fds.push_back(client_pollfd);
	this->_client_fds.push_back(client_fd);
	
	char	client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
	
	std::cout << "New client connected:" << std::endl;
	std::cout << "  - FD: " << client_fd << std::endl;
	std::cout << "  - IP: " << client_ip << std::endl;
	std::cout << "  - Port: " << ntohs(client_addr.sin_port) << std::endl;
	std::cout << "  - Total clients: " << this->_client_fds.size() << std::endl;
}

void	Server::handleClientData(int client_fd)
{
	std::cout << "...Handling data from client (FD: " << client_fd << ")..." << std::endl;

	char buffer[1024];

	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received < 0)
	{
		std::cerr << "Error: recv() failed for client " << client_fd << std::endl;
		return;
	}
	else if (bytes_received == 0)
	{
		std::cout << "Client (FD: " << client_fd << ") disconnected" << std::endl;
		removeClient(client_fd);
		return;
	}

	buffer[bytes_received] = '\0';	// Finish buffer of null

	_client_buffers[client_fd] += std::string(buffer, bytes_received);	// Add the new data to the client buffer
	
	processClientMessages(client_fd); // Process Complet messages
}

void	Server::removeClient(int client_fd)
{
	std::cout << "...Removing client (FD: " << client_fd << ")..." << std::endl;
	
	// Find and remove from _client_fds
	std::vector<int>::iterator it = std::find(this->_client_fds.begin(), this->_client_fds.end(), client_fd);
	
	if (it != this->_client_fds.end())
	{
		this->_client_fds.erase(it);
		std::cout << "Client removed from _client_fds" << std::endl;
	}
	else
		std::cerr << "Error: Client FD not found in _client_fds" << std::endl;
	
	// Find and remove from _poll_fds
	for (std::vector<struct pollfd>::iterator poll_it = this->_poll_fds.begin(); poll_it != this->_poll_fds.end(); ++poll_it)
	{
		if (poll_it->fd == client_fd)
		{
			this->_poll_fds.erase(poll_it);
			std::cout << "Client removed from _poll_fds" << std::endl;
			break;
		}
	}

	// Remove client buffer
	_client_buffers.erase(client_fd);
	std::cout << "Client buffer removed" << std::endl;

	if (close(client_fd) == -1)
		std::cerr << "Error: Failed to close client socket" << std::endl;
	else
		std::cout << "Client socket closed successfully" << std::endl;

	std::cout << "Client cleanup completed. Remaining clients: " << this->_client_fds.size() << std::endl;
}

int	Server::findClientIndex(int client_fd)
{
	for (size_t i = 0; i < this->_client_fds.size(); i++) // Search in _client_fds
	{
		if (this->_client_fds[i] == client_fd)
			return static_cast<int>(i);
	}
	return (-1);
}

void	Server::processClientMessages(int client_fd)
{
	std::string &buffer = _client_buffers[client_fd];
	size_t pos = 0;

	// Search messages ending in \r\n or \n
	while ((pos = buffer.find("\r\n")) != std::string::npos || (pos = buffer.find("\n")) != std::string::npos)
	{
		std::string message = buffer.substr(0, pos);

		if (buffer.substr(pos, 2) == "\r\n")	// Determine it is \r\n or \n to know how many characters to delete.
		{
			buffer.erase(0, pos + 2);			// Eliminate message + \r\n
		}
		else
		{
			buffer.erase(0, pos + 1);			// Eliminate message + \n
		}

		if (!message.empty())
		{
			std::cout << "Message received from FD" << client_fd << ": " << message << std::endl;
			parceIRCMessage(client_fd, message, ' '); // Usar espacio como delimitador para comandos IRC
		}
	}
}

void	Server::parceIRCMessage(int client_fd, const std::string &message, char delimiter)
{
	std::vector<std::string> tokens = splitMessage(message, delimiter);

	if (tokens.empty())
	{
		return;
	}

	std::string command = tokens[0];

	std::transform(command.begin(), command.end(), command.begin(), ::toupper);
	std::cout << "Command: " << command << std::endl;

	if (command == "PASS")
		PassCommand(client_fd, tokens);
	else if (command == "QUIT")
		QuitCommand(client_fd, tokens);
	else if (command == "NICK")
		NickCommand(client_fd, tokens);
	else if (command == "JOIN")
		JoinCommand(client_fd, tokens);
	/*else if (command == "KICK")
	else if (command == "INFO")
	else if (command == "TOPIC")
	else if (command == "PRIVMSG")
	else if (command == "INVITE")
	else if (command == "LIST")
	else if (command == "NAMES")
	else if (command == "MODE")*/
}