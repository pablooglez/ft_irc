/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:23:01 by pablogon          #+#    #+#             */
/*   Updated: 2025/06/18 20:02:49 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include <algorithm>
#include <sstream>

Channel::Channel()
{
	this->_user_limit = 0;
	this->_invite_only = false;
	this->_topic_protected = false;
	this->_has_password = false;
	this->_has_user_limit = false;
}

Channel::Channel(const std::string &name) 
{
	this->_name = name;
	this->_user_limit = 0;
	this->_invite_only = false;
	this->_topic_protected = false;
	this->_has_password = false;
	this->_has_user_limit = false;
}

Channel::Channel(const std::string &name, Client *creator)
{
	this->_name = name;
	this->_user_limit = 0;
	this->_invite_only = false;
	this->_topic_protected = false;
	this->_has_password = false;
	this->_has_user_limit = false;

	if (creator)
	{
		this->_clients.push_back(creator);
		this->_operators.push_back(creator);
	}
}

Channel::Channel(const Channel &obj)
{
	*this = obj;
}

Channel &Channel::operator=(const Channel &obj)
{
	if (this != &obj)
	{
		this->_name = obj._name;
		this->_topic = obj._topic;
		this->_password = obj._password;
		this->_user_limit = obj._user_limit;
		this->_clients = obj._clients;
		this->_operators = obj._operators;
		this->_invited = obj._invited;
		this->_invite_only = obj._invite_only;
		this->_topic_protected = obj._topic_protected;
		this->_has_password = obj._has_password;
		this->_has_user_limit = obj._has_user_limit;
	}
	return *this;
}

Channel::~Channel()
{
}

// GETTERS

const std::string &Channel::getName() const
{
	return this->_name;
}

const std::string &Channel::getTopic() const
{
	return this->_topic;
}

const std::string &Channel::getPassword() const
{
	return this->_password;
}

int &Channel::getUserLimit() const
{
	return const_cast<int&>(this->_user_limit);
}

const std::vector<Client*> &Channel::getClients() const
{
	return this->_clients;
}

const std::vector<Client*> &Channel::getOperators() const
{
	return this->_operators;
}

const std::vector<Client*> &Channel::getInvited() const
{
	return this->_invited;
}

// GETTERS (FOR MODES)

bool Channel::isInviteOnly() const
{
	return this->_invite_only;
}

bool Channel::isTopicProtected() const
{
	return this->_topic_protected;
}

bool Channel::hasPassword() const
{
	return this->_has_password;
}

bool Channel::hasUserLimit() const
{
	return this->_has_user_limit;
}

// Return string with active modes
std::string Channel::getModes() const
{
	std::string modes = "+";
	if (this->_invite_only)
		modes += "i";
	if (this->_topic_protected)
		modes += "t";
	if (this->_has_password)
		modes += "k";
	if (this->_has_user_limit)
		modes += "l";
	
	// If no modes are active, return empty string
	if (modes == "+")
		return "";

	return modes;
}

// SETTERS

void Channel::setTopic(const std::string &topic)
{
	this->_topic = topic;
}

void Channel::setPassword(const std::string &password)
{
	this->_password = password;
	this->_has_password = !password.empty();
}

void Channel::setUserLimit(int limit)
{
	this->_user_limit = limit;
	this->_has_user_limit = (limit > 0);
}

void Channel::setInvitedOnly(bool invite_only)
{
	this->_invite_only = invite_only;
}

void Channel::setTopicProtected(bool topic_protected)
{
	this->_topic_protected = topic_protected;
}

// USER MANAGEMENT

bool Channel::addClient(Client *client)
{
	if (!client)
		return false;
	
	// Verify if client is already in the channel
	if (hasClient(client))
		return false;
	
	// Check user limit if it is active
	if (this->_has_user_limit && (int)this->_clients.size() >= this->_user_limit)
		return false;
	
	this->_clients.push_back(client);
	return true;
}

bool Channel::removeClient(Client *client)
{
	if (!client)
		return false;
	
	// Search and delete clients
	std::vector<Client*>::iterator it = std::find(this->_clients.begin(), this->_clients.end(), client);
	if (it != this->_clients.end())
	{
		this->_clients.erase(it);

		removeOperator(client);
		removeInvited(client);

		return true;
	}
	
	return false;
}

bool Channel::hasClient(Client *client) const
{
	if (!client)
		return false;
	
	return std::find(this->_clients.begin(), this->_clients.end(), client) != this->_clients.end();
}

bool Channel::hasClient(const std::string &nickname) const
{
	for (std::vector<Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if ((*it)->getNickName() == nickname)
			return true;
	}
	return false;
}

Client* Channel::getClient(const std::string &nickname) const
{
	for (std::vector<Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if ((*it)->getNickName() == nickname)
			return *it;
	}
	return NULL;
}

int Channel::getClientCount() const
{
	return (int)this->_clients.size();
}

bool Channel::isEmpty() const
{
	return this->_clients.empty();
}

// OPERATOR MANAGEMENT

bool Channel::addOperator(Client* client)
{
	if (!client)
		return false;
	
	// Verify if you are already an operator
	if (isOperator(client))
		return false;
	
	// The customer must be in the channel to be an operator.
	if (!hasClient(client))
		return false;
	
	this->_operators.push_back(client);
	return true;
}

bool Channel::removeOperator(Client *client)
{
	if (!client)
		return false;
	
	std::vector<Client*>::iterator it = std::find(this->_operators.begin(), this->_operators.end(), client);
	if (it != this->_operators.end())
	{
		this->_operators.erase(it);
		return true;
	}
	
	return false;
}

bool Channel::isOperator(Client *client) const
{
	if (!client)
		return false;
	
	return std::find(this->_operators.begin(), this->_operators.end(), client) != this->_operators.end();
}

bool Channel::isOperator(const std::string &nickname) const
{
	for (std::vector<Client*>::const_iterator it = this->_operators.begin(); it != this->_operators.end(); ++it)
	{
		if ((*it)->getNickName() == nickname)
			return true;
	}
	return false;
}

// INVITATION MANAGEMENT (mode +i)

bool Channel::addInvited(Client* client)
{
	if (!client)
		return false;
	
	// Check if you are already invited
	if (isInvited(client))
		return false;
	
	this->_invited.push_back(client);
	return true;
}

bool Channel::removeInvited(Client* client)
{
	if (!client)
		return false;
	
	std::vector<Client*>::iterator it = std::find(this->_invited.begin(), this->_invited.end(), client);
	if (it != this->_invited.end())
	{
		this->_invited.erase(it);
		return true;
	}
	
	return false;
}

bool Channel::isInvited(Client* client) const
{
	if (!client)
		return false;
	
	return std::find(this->_invited.begin(), this->_invited.end(), client) != this->_invited.end();
}

// ACCESS CHECKS

bool Channel::canJoin(Client* client, const std::string& password) const
{
	if (!client)
		return false;
	
	// If client is already in the channel, he/she cannot join again.
	if (hasClient(client))
		return false;
	
	// Verify user limit
	if (this->_has_user_limit && (int)this->_clients.size() >= this->_user_limit)
		return false;
	
	// Verify password if the channel has one
	if (this->_has_password && this->_password != password)
		return false;
	
	// Verify invite-only mode
	if (this->_invite_only && !isInvited(client))
		return false;
	
	return true;
}

// COMMUNICATION

void Channel::broadcast(const std::string& message, Client* sender) const
{
	for (std::vector<Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (*it && *it != sender)	// Do not send the message to the sender
		{
			(*it)->sendMessage(message);
		}
	}
}

void Channel::broadcastToOthers(const std::string& message, Client* sender) const
{
	for (std::vector<Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (*it && *it != sender)	// Send only to other users, not to the sender
		{
			(*it)->sendMessage(message);
		}
	}
}

// UTILITIES

// Returns the list of users for the command NAMES
std::string Channel::getNamesReply() const
{
	std::string names;
	
	for (std::vector<Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (*it)
		{
			// Add operator prefix if applicable
			if (isOperator(*it))
				names += "@";
			
			names += (*it)->getNickName();
			
			// Add space if it is not the last item
			if (it + 1 != this->_clients.end())
				names += " ";
		}
	}
	
	return names;
}

// Returns string with the list of users
std::string Channel::getUserListString() const
{
	std::string userList;
	
	for (std::vector<Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (*it)
		{
			userList += (*it)->getNickName();
			if (it + 1 != this->_clients.end())
				userList += ", ";
		}
	}
	
	return userList;
}

// Channel information for debugging
std::string Channel::getChannelInfo() const
{
	std::string info;
	std::stringstream ss;
	
	info += "Channel: " + this->_name + "\n";
	info += "Topic: " + this->_topic + "\n";
	
	ss << this->_clients.size();
	info += "Users: " + ss.str() + "\n";
	
	ss.str("");
	ss << this->_operators.size();
	info += "Operators: " + ss.str() + "\n";
	
	info += "Modes: " + getModes() + "\n";
	
	return info;
}

// VALIDATIONS

// Static method to validate channel names
bool Channel::isValidChannelName(const std::string& name)
{
	// The channel name must start with # and be at least 2 characters long.
	if (name.length() < 2 || name[0] != '#')
		return false;
	
	// Verify valid characters (no spaces, no commas, no control characters)
	for (size_t i = 1; i < name.length(); ++i)
	{
		char c = name[i];
		if (c == ' ' || c == ',' || c == '\r' || c == '\n' || c == '\0' || c == 7)  // 7 = BELL
			return false;
	}
	
	// The name should not be too long (RFC suggests 50 characters).
	if (name.length() > 50)
		return false;
	
	return true;
}