/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:19:43 by albelope          #+#    #+#             */
/*   Updated: 2025/07/15 20:18:51 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

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