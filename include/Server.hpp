/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 18:37:38 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/06 18:54:23 by pablogon         ###   ########.fr       */
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
#include <arpa/inet.h> //inet_addr, inet_ntoa
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

class Server
{
	private:
			int				_port;
			std::string		_password;
			int				_server_fd;

			bool			_running;
			sockaddr_in		_server_addr;
	public:
			Server(int port, const std::string &password);
			~Server();


			int						GetPort() const;
			const std::string		&GetPassword() const;
			int						getServerFd() const;


			bool	setupSocket();	// Configure Socket
			void	start();		// Init Server
			void	stop();			// Stop Server
			void	cleanup();
};