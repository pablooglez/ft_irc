/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 12:36:00 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/15 20:34:21 by pablogon         ###   ########.fr       */
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
		std::string					_name;
		std::string					_topic;

		std::string					_key;
		bool						_inviteOnly;
		bool						_topicRestricted;
		size_t						_userLimit;
		bool						_hasPassword;
		bool						_hasUserLimit;

		std::vector<Client*>		_members;
		std::set<Client*>			_operators;
		std::set<Client*>			_invited;

	public:

		Channel();
		Channel(const std::string &name);
		Channel(const std::string &name, Client* creator);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
		~Channel();

		bool						isMember(Client* member) const;
		bool						addMember(Client* member);
		bool						leaveChannel(Client* member);
		bool						kickUser(Client* kicker, Client* target);
		bool						inviteUser(Client* inviter, Client* target);

		bool						addOperator(Client* client);
		bool 						removeOperator(Client* client);
		bool						isOperator(Client* client) const;
		bool						isOperator(const std::string& nickname) const;

		bool						addInvited(Client* client);
		bool						removeInvited(Client* client);
		bool						isInvited(Client* client) const;
		void						clearInvited();

		const std::vector<Client*>	&whoIsIn() const;
		bool						canJoin(Client* client, const std::string& password = "") const;
		bool						isEmpty() const;
		bool						isFull() const;
		int							getClientCount() const;
		int							getOperatorCount() const;

		Client*						getClientbyNickname(const std::string& nickname)const;
		bool						hasClient(const std::string& nickname)const;

		bool						canKick(Client* kicker, Client* target)const;
		bool						canSetTopic(Client* client) const;

		void						broadcast(const std::string& message, Client* sender = NULL) const;
		void						broadcastToOthers(const std::string& message, Client* sender) const;

		std::string					getNamesReply() const;
		std::string					getModes() const;
		std::string					getTopicReply() const;
		std::string					getUserListString() const;

		static bool					isValidChannelName(const std::string& name);

		const std::string			&getChannelName() const;
		const std::vector<Client*>	&getMembers() const;
		const std::string			&getPassword() const;
		const std::string			&getTopic() const;
		bool						isInviteOnly() const;
		bool 						isTopicRestricted() const;
		int							getUserLimit() const;
		bool						hasPassword() const;
		bool						hasUserLimit() const;

		void						setPassword(const std::string &password);
		void						setTopic(const std::string &topicName);
		void						setChannelName(const std::string &name);
		void						setInviteOnly(bool invite);
		void						setTopicRestricted(bool restricted);
		bool						setUserLimit(int limit);

		bool						removeClient(Client *client);
};