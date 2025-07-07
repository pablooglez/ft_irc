/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 12:36:00 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/07 21:08:02 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <set>
#include <string>
#include <vector>
#include <map>

class Server;
class Client;

class Channel
{
	private:
		// === CHANNEL IDENTIFICATION ===
		std::string					_name;							// Channel name (e.g., "#general")
		std::string					_topic;							// Channel topic

		// === CHANNEL MODES AND ACCESS CONTROL ===
		std::string					_key;							// Password for protected channels (MODE +k)
		bool						_inviteOnly;					// Invite-only flag (MODE +i)
		bool						_topicRestricted;				// Topic protection flag (MODE +t)
		size_t						_userLimit;						// Maximum users allowed (MODE +l)
		bool						_hasPassword;					// Flag indicating if password is set
		bool						_hasUserLimit;					// Flag indicating if user limit is active

		// === USER MANAGEMENT ===
		std::vector<Client*>		_members;						// List of all users in channel
		std::set<Client*>			_operators;						// Channel operators with special privileges
		std::set<Client*>			_invited;						// Users invited to join invite-only channel

	public:

		// === CONSTRUCTORS AND DESTRUCTOR ===
		Channel();
		Channel(const std::string &name);
		Channel(const std::string &name, Client* creator);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
		~Channel();
		
		// === MEMBER MANAGEMENT ===
		bool						isMember(Client* member) const;
		bool						addMember(Client* member);
		bool						leaveChannel(Client* member);
		bool						kickUser(Client* kicker, Client* target);
		bool						inviteUser(Client* inviter, Client* target);

		// === OPERATOR MANAGEMENT ===
		bool						addOperator(Client* client);
		bool 						removeOperator(Client* client);
		bool						isOperator(Client* client) const;
		bool						isOperator(const std::string& nickname) const;
		
		// === INVITATION MANAGEMENT ===
		bool						addInvited(Client* client);
		bool						removeInvited(Client* client);
		bool						isInvited(Client* client) const;
		void						clearInvited();

		// === CHANNEL QUERIES AND VALIDATION ===
		const std::vector<Client*>	&whoIsIn() const;
		bool						canJoin(Client* client, const std::string& password = "") const;
		bool						isEmpty() const;
		bool						isFull() const;
		int							getClientCount() const;
		int							getOperatorCount() const;

		// === CLIENT SEARCH METHODS ===
		Client*						getClientbyNickname(const std::string& nickname)const;
		bool						hasClient(const std::string& nickname)const;

		// === PERMISSION VALIDATION ===
		bool						canKick(Client* kicker, Client* target)const;
		bool						canSetTopic(Client* client) const;

		// === COMMUNICATION METHODS ===
		void						broadcast(const std::string& message, Client* sender = NULL) const;
		void						broadcastToOthers(const std::string& message, Client* sender) const;

		// === IRC PROTOCOL RESPONSES ===
		std::string					getNamesReply() const;
		std::string					getModes() const;
		std::string					getTopicReply() const;
		std::string					getUserListString() const;

		// === STATIC VALIDATION METHODS ===
		static bool					isValidChannelName(const std::string& name);

		// === GETTERS (READ-ONLY ACCESS) ===
		const std::string			&getChannelName() const;
		const std::vector<Client*>	&getMembers() const;
		const std::string			&getPassword() const;
		const std::string			&getTopic() const;
		bool						isInviteOnly() const;
		bool 						isTopicRestricted() const;
		int							getUserLimit() const;
		bool						hasPassword() const;
		bool						hasUserLimit() const;
		
		// === SETTERS (MODIFY CHANNEL STATE) ===
		void						setPassword(const std::string &password);
		void						setTopic(const std::string &topicName);
		void						setChannelName(const std::string &name);
		void						setInviteOnly(bool invite);
		void						setTopicRestricted(bool restricted);
		bool						setUserLimit(int limit);

		// Additional functionality
		bool						removeClient(Client *client);
};