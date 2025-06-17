/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 18:37:38 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/17 16:44:36 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream> // std::cout, std::cerr
#include <cstring> // std::string
#include <cstdlib> // exit, atoi
#include <vector> // std::vector
#include <map> // std::map
#include <algorithm> // std::find, std::remove
#include <sstream> // std::stringstream
#include <exception> // exceptions

// Sockets and Network
#include <sys/socket.h> // socket, blind, listen, accept, send, recv, setsockopt, getsockname
#include <netinet/in.h> // struct sockaddr_in, INADDR_ANY, htons, htonl, ntohs, ntohl
#include <arpa/inet.h> // inet_ntop, inet_pton
#include <netdb.h> // getprotobyname, gethostbyname, getaddrinfo, freeaddrinfo

// I/O
#include <poll.h> // poll()

// File Control
#include <fcntl.h> // fcntl()
#include <unistd.h> // close(), lseek()
#include <sys/stat.h> // fstat

// Signals
#include <csignal>

// Time
#include <ctime> // time_t, time()

extern int	g_global;

class Server
{
	private:
			int							_port;
			std::string					_password;
			int							_server_fd;

			bool						_running;
			sockaddr_in					_server_addr;

			std::vector<struct pollfd>	_poll_fds;		// Arrays of fds for poll()
			std::vector<int>			_client_fds;	// List of clients FDs (for mapping)

			std::map<int, std::string> _client_buffers;	// Buffer for Client
	public:
			Server(int port, const std::string &password);
			~Server();


			int						GetPort() const;
			const std::string		&GetPassword() const;
			int						getServerFd() const;


			bool	setupSocket();	// Configure Socket
			void	start();		// Init Server
			void	stop();			// Stop Server
			void	cleanup();		// Clean resources


			static void	handleSignal(int signal); // Signals

			// Methos poll()
			void	runServerLoop();						// Array fds for poll()
			void	handleNewConnection();					// Handle a new connection
			void	handleClientData(int client_fd);		// Handle Client Data
			void	removeClient(int client_fd);			// Remove Client
			int		findClientIndex(int client_fd);			// Find Client Index

			void	processClientMessages(int client_fd);	// Process Messages
			void	parceIRCMessage(int client_fd, const std::string &message, char delimiter); // Parce IRC Messages
			


			// UTILS
			std::vector<std::string> splitMessage(const std::string &message, char delimiter); // Split the message


			// COMMANDS
			void	PassCommand(int client_fd, const std::vector<std::string> &tokens);
			void	QuitCommand(int client_fd, const std::vector<std::string> &tokens);
			void	NickCommand(int client_fd, const std::vector<std::string> &tokens);
};