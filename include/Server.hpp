/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 18:37:38 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/07 21:02:30 by pablogon         ###   ########.fr       */
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

			// Server main loop
			void	runServerLoop();
			void	handleNewConnection();
			void	handleClientData(int client_fd);
			void	removeClient(int client_fd);
			int		findClientIndex(int client_fd);

			void	processClientMessages(int client_fd);
			void	parceIRCMessage(int client_fd, const std::string &message, char delimiter);

			
			// CLIENT MANAGEMENT
			void		createClientObject(int client_fd, const std::string &hostname);
			Client*		findClientByFd(int client_fd);
			Client*		findClientByNickname(const std::string &nickname);
			bool		isNicknameInUse(const std::string &nickname);
			bool		isValidNickname(const std::string &nickname);

			// CHANNEL MANAGEMENT
			Channel*	findOrCreateChannel(const std::string &channel_name);
			Channel*	findChannel(const std::string &channel_name);
			bool		channelExists(const std::string &channel_name);
			void		removeChannelIfEmpty(const std::string &channel_name);

			// COMMANDS
			void		PassCommand(int client_fd, const std::vector<std::string> &tokens);
			void		NickCommand(int client_fd, const std::vector<std::string> &tokens);
			void		UserCommand(int client_fd, const std::vector<std::string> &tokens);
			void		QuitCommand(int client_fd, const std::vector<std::string> &tokens);
			void		JoinCommand(int client_fd, const std::vector<std::string> &tokens);
			void		TopicCommand(int client_fd, const std::vector<std::string> &tokens);
			void		PrivmsgCommand(int client_fd, const std::vector<std::string> &tokens);
			void		InfoCommand(int client_fd, std::vector<std::string> &tokens);
			void		KickCommand(int client_fd, std::vector<std::string> &tokens);
			void		InviteCommand(int client_fd, std::vector<std::string> &tokens);
			void		NamesCommand(int client_fd, std::vector<std::string> &tokens);
			void		ListCommand(int client_fd, std::vector<std::string> &tokens);
			void		ModesCommand(int client_fd, std::vector<std::string> &tokens);

			// MODE HANDLERS
			void		handleInvideMode(Channel *channel, Client *client, bool checkMode);
			void		handleTopicMode(Channel *channel, Client *client, bool checkMode);
			void		handleLimitMode(Channel *channel, Client *client, bool checkMode, const std::string& parameter);
			void		handleKeyMode(Channel *channel, Client *client, bool checkMode, const std::string& parameter);
			void		handleUnknownMode(Channel *channel, Client *client, bool checkMode);

			// WELCOME MESSAGES
			void		sendWelcomeMessages(int client_fd);

			// UTILS
			std::vector<std::string> splitMessage(const std::string &message, char delimiter);
};