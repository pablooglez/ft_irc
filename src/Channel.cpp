/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 12:48:09 by albelope          #+#    #+#             */
/*   Updated: 2025/06/19 13:11:00 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

// === CONSTRUCTORS ===

Channel::Channel()
	: _name(""), _topic(""), _key(""), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(false),
	_hasUserLimit(false) {}

Channel::Channel(const std::string &name)
	: _name(name), _topic(""), _key(""), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(false),
	_hasUserLimit(false) {}

Channel::Channel(const std::string &name, const std::string &password)
	: _name(name), _topic(""), _key(password), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(!password.empty()),
	_hasUserLimit(false) {}

Channel::Channel(const std::string &name, Client* creator)
	: _name(name), _topic(""), _key(""), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(false),
	_hasUserLimit(false)
{
	if (creator)
	{
		_members.push_back(creator);
		_operators.insert(creator);
	}
}