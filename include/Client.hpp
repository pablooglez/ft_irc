/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 13:23:19 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/07 21:07:11 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sys/socket.h>

class Channel;

class Client
{
	private:
			int						_fd;				// FD Socket
			std::string				_nickname;			// Nick User
			std::string				_username;			// Name User
			std::string				_hostname;			// Address IP/Hostname

			bool					_is_authenticated;	// PASS Successfully
			bool					_has_nickname;		// Have NICK
			bool					_has_userinfo;		//Have USER
			bool					_is_registered;		// Succesfully Registered (PASS + NICK + USER)

			std::vector<Channel*>	_channels;			// Channels where the user is
			std::string				_buffer;			// Buffer for incomplete messages

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
};