/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:19:43 by albelope          #+#    #+#             */
/*   Updated: 2025/07/14 18:00:35 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>	// std::string, std::cout, std::cerr
#include <map>	// std::map
#include <vector>	// std::vector
#include <string>	// std::string
#include <sys/socket.h>	// socket, send, recv
#include <netinet/in.h>	// sockaddr_in, htons
#include <arpa/inet.h>	//inet_pton
#include <unistd.h>	// close, sleep
#include <cstring>	// memset
#include <cstdlib>	// exit, atoi

class Bot
{
	private:
			std::string			_ip;
			int					_port;
			std::string			_pass;
			std::string			_nick;
			std::string			_user;
			std::string			_channel;
			std::string			_bufferBot;
			int					_fd;

	public:
			Bot(const std::string &ip, int port, const std::string &pass, const std::string &channel);
			~Bot();

		std::string getChannel() const;

		bool	safeSend(const std::string& msg);
		bool	handlePing(const std::string& line);
		void	processMessage(const std::string& line);
		void	start();
};