/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 18:37:38 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/28 22:51:54 by albelope         ###   ########.fr       */
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

// Sockets and Network
#include <sys/socket.h> // socket, blind, listen, accept, send, recv, setsockopt, getsockname
#include <netinet/in.h> // struct sockaddr_in, INADDR_ANY, htons, htonl, ntohs, ntohl
#include <arpa/inet.h> // inet_ntop, inet_pton

// I/O
#include <poll.h> // poll()

// File Control
#include <fcntl.h> // fcntl()
#include <unistd.h> // close(), lseek()

// Signals
#include <csignal>

// Time
#include <ctime> // time_t, time()

extern int	g_global;

# include "Client.hpp"
# include "Channel.hpp"
# include "RPL.hpp"

class Client;
class Channel;
class RPL;

class Server
{
	private:
			int							_port;
			std::string					_password;
			std::string					_server_name;
			int							_server_fd;
	
			bool						_running;
			sockaddr_in					_server_addr;
	
			std::vector<struct pollfd>	_poll_fds;		// Arrays of fds for poll()
			std::vector<int>			_client_fds;	// List of clients FDs (for mapping)

			std::map<int, std::string> _client_buffers;	// Buffer for Client
			std::map<int, Client>		_clients;		// Map of Client objects (fd -> Client)

			std::map<std::string, Channel> _channels;

	public:
			Server(int port, const std::string &password);
			~Server();

			
			int						GetPort() const;
			const std::string		&GetPassword() const;
			const std::string&		getServerName() const;		// Get server name for RPL messages
			int						getServerFd() const;
			std::string				getClientNick(int client_fd) const;	// Get client nickname or "*" if none


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

			
			// CLIENT MANAGEMENT
			void		createClientObject(int client_fd, const std::string &hostname); // Create Client object
			Client*		findClientByFd(int client_fd);									// Find client by file descriptor
			bool		isNicknameInUse(const std::string &nickname);					// Check if nickname is already taken
			bool		isValidNickname(const std::string &nickname);					// Validate nickname format
			
			void		PrivmsgCommand(int client_fd, const std::vector<std::string> &tokens);


			




			
			
			// COMMANDS
			void	PassCommand(int client_fd, const std::vector<std::string> &tokens);
			void	NickCommand(int client_fd, const std::vector<std::string> &tokens);
			void	UserCommand(int client_fd, const std::vector<std::string> &tokens);
			void	QuitCommand(int client_fd, const std::vector<std::string> &tokens);
			void	JoinCommand(int client_fd, const std::vector<std::string> &tokens);




			void 	InfoCommand(int client_fd, std::vector<std::string> &tokens);

			void    KickCommand(int client_fd, std::vector<std::string> &tokens);

			// WELCOME MESSAGES
			void	sendWelcomeMessages(int client_fd);	// Send welcome messages to registered client

			// UTILS
			std::vector<std::string> splitMessage(const std::string &message, char delimiter); // Split the message
			
			
			
			
			
			
			
			
			
			
			
			//======================================================================================
			// === FUNCIONES AUXILIARES PARA PRIVMSG - CHECKEAR CON PABLO ===
			//======================================================================================
	
			
			Client*		findClientByNickname(const std::string& nickname);			// PENDIENTE: Find clientE by nickname - Returns pointer or nullptr
			Channel*	findChannelByName(const std::string& name);					// PENDIENTE: Find channel by name - Returns pointer or nullptr  
			bool		channelExists(const std::string& name);						// PENDIENTE: Checker if channel exists - Returns true/false
								
			//======================================================================================
};