/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 12:36:00 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/19 15:50:27 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <sstream> //std::stringstream

class Client;

class Channel
{
	private:
			std::string			_name;					// Name of channel (start in #)
			std::string			_topic;					// Topic of channel
			std::string			_password;				// Password of channel (mode +k)

			int					_user_limit;			// Limit of users (mode +l)

			std::vector<Client*>	_clients;			// Users in the channel
			std::vector<Client*>	_operators;			// Operators in the channel
			std::vector<Client*>	_invited;			// Guest users (mode +i)

			// Channel modes
			bool					_invite_only;		// Mode +i
			bool					_topic_protected;	// Mode +t (Only opts can change the topic)
			bool					_has_password;		// Mode +k
			bool					_has_user_limit;	// Mode +l

	public:
			Channel();
			Channel(const std::string &name);
			Channel(const std::string &name, Client *creator);
			Channel(const Channel &obj);
			Channel &operator=(const Channel &obj);
			~Channel();

			// GETTERS
			const std::string				&getName() const;
			const std::string				&getTopic() const;
			const std::string				&getPassword() const;
			int								getUserLimit() const;
			const std::vector<Client*>		&getClients() const;
			const std::vector<Client*>		&getOperators() const;
			const std::vector<Client*>		&getInvited() const;

			// GETTERS (FOR MODES)
			bool							isInviteOnly() const;
			bool							isTopicProtected() const;
			bool							hasPassword() const;
			bool							hasUserLimit() const;
			std::string						getModes() const;			// Returns string with active modes

			// SETTERS
			void							setTopic(const std::string &topic);
			void							setPassword(const std::string &password);
			void							setUserLimit(int limit);
			void							setInvitedOnly(bool invite_only);
			void							setTopicProtected(bool topic_protected);

			//USER MANAGEMENT
			bool							addClient(Client *client);
			bool							removeClient(Client *client);
			bool							hasClient(Client *client) const;
			bool							hasClient(const std::string &nickname) const;
			Client*							getClient(const std::string &nickname) const;
			int								getClientCount() const;
			bool							isEmpty() const;


			// OPERATOR MANAGEMENT
			bool							addOperator(Client*client);
			bool							removeOperator(Client *client);
			bool							isOperator(Client *client) const;
			bool							isOperator(const std::string &nickname) const;

			//MANAGEMENT OF INVITATIONS (MODE +i)
			bool	addInvited(Client* client);
			bool	removeInvited(Client* client);
			bool	isInvited(Client* client) const;

			// ACCESS CHECKS
			bool	canJoin(Client* client, const std::string& password = "") const;

			// Communication
			void	broadcast(const std::string& message, Client* sender = NULL) const;
			void	broadcastToOthers(const std::string& message, Client* sender) const;

			// UTILITIES
			std::string getNamesReply() const;			// User list for NAMES
			std::string getUserListString() const;		// String with list of users
			std::string getChannelInfo() const;			// Debugging channel information
			
			// VALIDATIONS
			static bool isValidChannelName(const std::string& name);
};