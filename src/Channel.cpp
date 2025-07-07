/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:23:01 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/07 21:39:53 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include <sstream>

// === CONSTRUCTORS ===

Channel::Channel()
	: _name(""), _topic(""), _key(""), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(false),
	_hasUserLimit(false) {}

Channel::Channel(const std::string &name)
	: _name(name), _topic(""), _key(""), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(false),
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

Channel::Channel(const Channel &other) 
 	: _name(other._name),  _topic(other._topic), _key(other._key),
	_inviteOnly(other._inviteOnly), _topicRestricted(other._topicRestricted),
	_userLimit(other._userLimit), _hasPassword(other._hasPassword),
	_hasUserLimit(other._hasUserLimit) {}


Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_topic = other._topic;
		_key = other._key;
		_inviteOnly = other._inviteOnly;
		_topicRestricted = other._topicRestricted;
		_userLimit = other._userLimit;
		_hasPassword = other._hasPassword;
		_hasUserLimit = other._hasUserLimit;
	}
	return *this;
}

Channel::~Channel()
{
	
}

 	// ===========================================
	// ======		MEMBER MANAGEMET 	==========
	// ===========================================


bool	Channel::isMember(Client* member) const
{
	if (member == NULL)
		return false;
	return hasClient(member->getNickName());
}

/*	isMember(Client* member):
		→ Solo responde si el cliente ya está en el canal (_members).
		→ No hace ningún cambio.
		→ Se usa para saber si tiene sentido realizar ciertas acciones:
		   - evitar duplicados al hacer JOIN
		   - comprobar si alguien está dentro antes de hacer KICK, etc. */

bool	Channel::addMember(Client* member)
{
	if (member == NULL)
		return false;
	if (isMember(member))
		return false;

		_members.push_back(member);
	return true;
}

bool	Channel::leaveChannel(Client* member)
{
	if (member == NULL)
		return false;
	if (!isMember(member))
		return false;

	_members.erase(std::remove(_members.begin(), _members.end(), member), _members.end());
	_operators.erase(member);
	_invited.erase(member);
	return true;
}

bool	Channel::kickUser(Client* kicker, Client* target)
{
	if (kicker == NULL || target == NULL)
		return false;
	if (!isOperator(kicker))
		return false;
	if (!isMember(target))
		return false;
	if (kicker == target)
		return false;

	leaveChannel(target);
	return true;
}

bool	Channel::inviteUser(Client* inviter, Client* target)
{
	if (inviter == NULL || target == NULL)
		return false;
	if (!isOperator(inviter))
		return false;
	if (isMember(target))
		return false;
	if (!isMember(inviter))
		return false;
	if (inviter == target)
		return false;
	_invited.insert(target);
	return true;
}

// === OPERATOR MANAGEMENT ===

bool	Channel::addOperator(Client* client)
{
	if (client == NULL)
		return false;
	if (!isMember(client))
		return false;

	_operators.insert(client);
	return true;
}

bool	Channel::removeOperator(Client* client)
{
	if (client == NULL)
		return false;
	if (!isOperator(client))
		return false;

	_operators.erase(client);
	return true;
}

bool	Channel::isOperator(Client* client) const
{
	if (client == NULL)
		return false;
	return _operators.find(client) != _operators.end();
}

bool	Channel::isOperator(const std::string& nickname) const
{
	if (nickname.empty())
		return false;
	for (std::set<Client*>::const_iterator opIt = _operators.begin(); opIt != _operators.end(); ++opIt)
	{
		if ((*opIt)->getNickName() == nickname)
			return true;
	}
	return false;
}

// === INVITATION MANAGEMENT ===

bool	Channel::addInvited(Client* client)
{
	if (client == NULL)
		return false;
	_invited.insert(client);
	return true;
}

bool	Channel::removeInvited(Client* client)
{
	if (client == NULL)
		return false;
	if (_invited.find(client) == _invited.end())
		return false;
	_invited.erase(client);
	return true;
}

bool	Channel::isInvited(Client* client) const
{
	if (client == NULL)
		return false;
	if (_invited.find(client) == _invited.end())
		return false;
	return true;
}

void	Channel::clearInvited()
{
	_invited.clear();
}

// ===============================================
// ========= CHANNEL QUERIES AND VALIDATION ======
// ===============================================

const	std::vector<Client*> &Channel::whoIsIn() const
{
	return _members;
}

bool	Channel::canJoin(Client* client, const std::string& password) const
{
	if (client == NULL)
		return false;
	if (isMember(client))
		return false;
	if (_hasPassword)
	{
		if (_key != password)
			return false;
	}
	if (_inviteOnly)
	{
		if (!isInvited(client))
			return false;
	}
	if (_hasUserLimit)
	{
		if (_members.size() >= _userLimit)
			return false;
	}
	return true;
}

bool	Channel::isEmpty() const
{
	return _members.empty();
}

bool	Channel::isFull() const
{
	if (!_hasUserLimit)
		return false;
	if (_members.size() >= _userLimit)
		return true;
	return false;
}

int		Channel::getClientCount() const
{
	return _members.size();
}

int		Channel::getOperatorCount() const
{
	return _operators.size();
}

// === CLIENT SEARCH METHODS ===

Client* Channel::getClientbyNickname(const std::string& nickname) const
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i]->getNickName() == nickname)
			return _members[i];
	}
	return NULL;
}

bool	Channel::hasClient(const std::string& nickname) const
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i]->getNickName() == nickname)
			return true;
	}
	return false;
}
/*
 * getClientbyNickname() vs hasClient()
 * Ambas recorren _members buscando un nickname, pero no hacen lo mismo:
 * ------------------------hasClient(nickname)--------------------------
 *     → Devuelve true/false. Solo comprueba si alguien está en el canal.
 *     → Útil para validaciones rápidas:
 *         · NAMES → ¿Está este usuario?
 *         · PRIVMSG → ¿Puede escribir aquí?
 *         · Logs/debug → ¿Quién está conectado
 * ---------------------- getClientbyNickname(nickname)-------------------
 *     → Devuelve un Client* o NULL. Te da el objeto si está.
 *     → Útil cuando necesitas usar ese cliente:
 *         · KICK → para echarlo
 *         · MODE → para cambiarle permisos
 *         · PRIVMSG directo → para enviarle datos
 * -----------Analogía-------------------------------------------------
 *   hasClient()          → "¿Está Juan en casa?"
 *   getClientbyNickname()→ "Pásame a Juan, quiero hablar con él"
 */



	// ===============================================
	// ========== PERMISSION VALIDATION ==============
	// ===============================================
	

bool	Channel::canKick(Client* kicker, Client* target) const
{
	if (kicker == NULL || target == NULL)
		return false;
	if (kicker == target)
		return false;
	if (!isOperator(kicker))
		return false;
	if (!isMember(target))
		return false;
	return true;
}

bool	Channel::canSetTopic(Client* client) const
{
	if (client == NULL)
		return false;
	if (!isMember(client))
		return false;
	if (_topicRestricted)
	{
		if (!isOperator(client))
			return false;
	}
	return true;
}

// === COMMUNICATION METHODS ===

void	Channel::broadcast(const std::string& message, Client* sender) const
{
	(void)sender;
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == NULL)
			continue;
		_members[i]->sendMessage(message);
	}
}

void	Channel::broadcastToOthers(const std::string& message, Client* sender) const
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == NULL)
			continue;
		if (_members[i] == sender)
			continue;
		_members[i]->sendMessage(message);
	}
}

// === IRC PROTOCOL RESPONSES ===


std::string		Channel::getNamesReply() const
{
	if (_members.empty())
		return "";
		
	std::string result;
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (isOperator(_members[i]))
			result += "@";
		result += _members[i]->getNickName();
		result += " ";
	}
	return result;
}

std::string		Channel::getModes() const
{
	
	std::string allmodes = "+";
	if (_inviteOnly)
		allmodes = allmodes + "i";
	if (_topicRestricted)
		allmodes = allmodes + "t";
	if (_hasPassword)
		allmodes = allmodes + "k";
	if (_hasUserLimit)
		allmodes = allmodes + "l";
	return allmodes;
}

std::string		Channel::getTopicReply() const
{
	return _topic;
}

std::string		Channel::getUserListString() const
{
	std::string	showNicknames;

	if (!_members.empty())
	{
		for (size_t i = 0; i < _members.size(); i++)
		{
			showNicknames = showNicknames + _members[i]->getNickName() + " ";
		}
	}
	else
	{
		showNicknames = "No Users";
	}
	return showNicknames;
}

	// ===============================================
	// ========== STATIC VALIDATION METHODS ==========
	// ===============================================
	
bool		Channel::isValidChannelName(const std::string& name)
{
	if (name.empty())
		return false;
	if (name.length() < 2)
		return false;
	if (name[0] != '#')
		return false;
	if (name.length() > 50)
		return false;
	if (name == "#SERVER" || name == "#NULL" || name == "#ADMIN" 
    || name == "#OPER" || name == "#ROOT")
		return false;
	for (size_t i = 1; i < name.length(); i++)
	{
		if (name[i] == ' ' || name[i] == ',' || name[i] == '\n'
				|| name[i] == '\r' || name[i] == '\0' || name[i] == '\t'
				|| name[i] == ':' || name[i] == '!' || name[i] == '@'
				|| name[i] == '&' || name[i] == '+')
			return false;
	}
	return true;
}


	// ===============================================
	// ========== GETTERS (READ-ONLY ACCESS) =========
	// ===============================================

const std::string& Channel::getChannelName() const
{
	return _name;
}

const std::vector<Client*>& Channel::getMembers() const
{
	return _members;
}

const std::string& Channel::getPassword() const
{
	return _key;
}

const std::string& Channel::getTopic() const
{
	return _topic;
}

bool	Channel::isInviteOnly() const
{
	return _inviteOnly;
}

bool	Channel::isTopicRestricted() const
{
	return _topicRestricted;
}

int		Channel::getUserLimit() const
{
	if (_hasUserLimit)
		return _userLimit;
	return 0;
}

bool	Channel::hasPassword() const
{
	return _hasPassword;
}

bool	Channel::hasUserLimit() const
{
	return _hasUserLimit;
}

	// ===============================================
	// ======= SETTERS (MODIFY CHANNEL STATE) ========
	// ===============================================

void	Channel::setPassword(const std::string &password)
{
	_key = password;
	//actualizamos el flag
	_hasPassword = !password.empty();
}
/*
Cambiar contraseña del canal (modo +k):
- Si metes una string (ej: "clave123") ⇒ activa el modo +k y guarda la clave.
- Si metes "" ⇒ desactiva el modo +k y borra la _key.
¿Por que y para que !password.empty()?
- Si la string no esta vacía -> hay contraseña → _hasPassword = true
- Si está vacía → no hay contraseña → _hasPassword = false
 _key y _hasPassword siempre estány deben estar sincronizados.
Solo los Op pueden hacer esto.
*/

void	Channel::setTopic(const std::string &topicName)
{
	_topic = topicName;
	
}

void	Channel::setChannelName(const std::string &name)
{
	_name = name;	
}

void	Channel::setInviteOnly(bool invite)
{
	_inviteOnly = invite;
}
/*
setInviteOnly(bool invite):
Esto activa o desactiva el modo +i del canal. 
Si pones true, solo pueden entrar los que estén invitados. Si pones false, entra quien quiera.
Lo típico de MODE +i o -i llama a esto. Solo los ops pueden usarlo. 
_inviteOnly es el bool que dice si el canal está cerrado, y _invited es la lista de los que tienen pase para entrar.
Ejo: pones setInviteOnly(true), luego haces inviteUser("alice") y ella entra, pero bob no porque no está invitado.
*/

void	Channel::setTopicRestricted(bool restricted)
{
	_topicRestricted = restricted;
}

bool	Channel::setUserLimit(int limit)
{
	if (limit < 0)
		return false;
	_userLimit = limit;
	_hasUserLimit = (limit > 0);
	return true;
}

bool Channel::removeClient(Client *client)
{
	if (!client)
		return false;
	
	std::vector<Client*>::iterator it = std::find(this->_members.begin(), this->_members.end(), client);
	if (it != this->_members.end())
	{
		this->_members.erase(it);
		removeOperator(client);
		removeInvited(client);
		return true;
	}
	
	return false;
}

