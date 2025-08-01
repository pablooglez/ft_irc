/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 13:23:19 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/15 20:34:44 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sys/socket.h>
#ifdef BONUS
#include <map>
#include <fstream>
#endif

class Channel;

class Client
{
	private:
			int						_fd;
			std::string				_nickname;
			std::string				_username;
			std::string				_hostname;

			bool					_is_authenticated;
			bool					_has_nickname;
			bool					_has_userinfo;
			bool					_is_registered;

			std::vector<Channel*>	_channels;
			std::string				_buffer;

			#ifdef BONUS
			std::map<std::string, std::string> _receivedFiles;
			#endif

	public:
			Client();
			Client(int fd, const std::string &hostname);
			Client(const Client &obj);
			Client &operator=(const Client &obj);
			~Client();

			int								getFd() const;
			const std::string				&getNickName() const;
			const std::string				&getUserName() const;
			const std::string				&getHostName() const;
			bool							isAuthenticated() const;
			bool							hasNickName() const;
			bool							hasUserInfo() const;
			bool							isRegistered() const;
			const std::vector<Channel*>		&getChannels()const;
			const std::string				&getBuffer() const;

			// SETTERS
			void	setNickName(const std::string &nickname);
			void	setUserName(const std::string &username);
			void	setHostName(const std::string &hostname);
			void	setAuthenticated(bool autheticated);
			void	setHasNickName(bool has_nickname);
			void	setHasUserInfo(bool has_userinfo);
			void	setBuffer(const std::string &buffer);

			// METHODS
			void	updateRegistrationStatus();
			bool	isFullyRegistered() const;

			// CHANNEL MANAGEMENT
			void	addChannel(Channel *channel);
			void	removeChannel(Channel *channel);
			bool	isInChannel(Channel *channel) const;
			bool	isInChannel(const std::string &channel_name) const;

			// COMMUNICATION
			bool	sendMessage(const std::string &message) const;
			void	appendtoBuffer(const std::string &data);
			void	clearBuffer();

			// FORMATTING UTILITIES
			std::string	getPrefix() const;

			// Send File
			#ifdef BONUS
			void	handleFileChunk(const std::string& filename, const std::string& data);
			void	handleFileEnd(const std::string& filename);
			#endif
};