/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:19:42 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/09 13:53:37 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include <fstream>

#ifdef BONUS
# include "../bonus/Base64.hpp"
#endif


Client::Client()
{
	this->_fd = -1;
	this->_is_authenticated = false;
	this->_has_nickname = false;
	this->_has_userinfo = false;
	this->_is_registered = false;
}

Client::Client(int fd, const std::string &hostname)
{
	this->_fd = fd;
	this->_hostname = hostname;
	this->_is_authenticated = false;
	this->_has_nickname = false;
	this->_has_userinfo = false;
	this->_is_registered = false;
}

Client::Client(const Client &obj)
{
	*this = obj;
}

Client &Client::operator=(const Client &obj)
{
	if (this != &obj)
	{
		this->_fd = obj._fd;
		this->_nickname = obj._nickname;
		this->_username = obj._username;
		this->_hostname = obj._hostname;
		this->_is_authenticated = obj._is_authenticated;
		this->_has_nickname = obj._has_nickname;
		this->_has_userinfo = obj._has_userinfo;
		this->_is_registered = obj._is_registered;
		this->_channels = obj._channels;
		this->_buffer = obj._buffer;
	}
	return (*this);
}

Client::~Client()
{
	this->_channels.clear();
}

//GETTERS
int	Client::getFd() const
{
	return this->_fd;
}

const std::string &Client::getNickName() const
{
	return this->_nickname;
}

const std::string &Client::getUserName() const
{
	return this->_username;
}

const std::string &Client::getHostName() const
{
	return this->_hostname;
}

bool Client::isAuthenticated() const
{
	return this->_is_authenticated;
}

bool Client::hasNickName() const
{
	return this->_has_nickname;
}

bool Client::hasUserInfo() const
{
	return this->_has_userinfo;
}

bool Client::isRegistered() const
{
	return this->_is_registered;
}

const std::vector<Channel*> &Client::getChannels() const
{
	return this->_channels;
}
const std::string &Client::getBuffer() const
{
	return this->_buffer;
}

//SETTERS
void	Client::setNickName(const std::string &nickname)
{
	this->_nickname = nickname;
	this->_has_nickname = !nickname.empty();
	updateRegistrationStatus();
}

void	Client::setUserName(const std::string &username)
{
	this->_username = username;
}

void	Client::setHostName(const std::string &hostname)
{
	this->_hostname = hostname;
}

void	Client::setAuthenticated(bool autheticated)
{
	this->_is_authenticated = autheticated;
}

void	Client::setHasNickName(bool has_nickname)
{
	this->_has_nickname = has_nickname;
}

void	Client::setHasUserInfo(bool has_userinfo)
{
	this->_has_userinfo = has_userinfo;
}

void	Client::setBuffer(const std::string &buffer)
{
	this->_buffer = buffer;
}

// METHOS
void	Client::updateRegistrationStatus()
{
	this->_is_registered = this->_is_authenticated && this->_has_nickname && this->_has_userinfo;
}

bool	Client::isFullyRegistered() const
{
	return this->_is_registered;
}

// CHANNEL MANAGEMENT
void	Client::addChannel(Channel *channel)
{
	if (channel && std::find(this->_channels.begin(), this->_channels.end(), channel) == this->_channels.end())
	{
		this->_channels.push_back(channel);
	}
}

void	Client::removeChannel(Channel *channel)
{
	std::vector<Channel*>::iterator it = std::find(this->_channels.begin(), this->_channels.end(), channel);
	if (it != this->_channels.end())
	{
		this->_channels.erase(it);
	}
}

bool	Client::isInChannel(Channel *channel) const
{
	return std::find(this->_channels.begin(), this->_channels.end(), channel) != this->_channels.end();
}

bool	Client::isInChannel(const std::string &channel_name) const
{
	for (std::vector<Channel*>::const_iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
	{
		if ((*it)->getChannelName() == channel_name)
			return true;
	}
	return false;
}

// COMMUNICATION
bool	Client::sendMessage(const std::string &message) const
{
	if (this->_fd == -1)
		return false;

	#ifdef BONUS
		// Oculta los mensajes internos del sistema de archivos
		if (message.find("FILECHUNK") != std::string::npos)
			return true; // No se envía nada al socket, pero se considera éxito
	#endif
	
	std::string formatted_message = message;

	if (formatted_message.substr(formatted_message.length() - 2) != "\r\n")
	{
		formatted_message += "\r\n";
	}

	ssize_t bytes_sent = send(this->_fd, formatted_message.c_str(), formatted_message.length(), 0);

	if (bytes_sent == -1)
	{
		std::cerr << "Error to sending message to client" << this->_fd << std::endl;
		return false;
	}
	return true;
}

void	Client::appendtoBuffer(const std::string &data)
{
	this->_buffer += data;
}

void	Client::clearBuffer()
{
	this->_buffer.clear();
}

// FORMATTING UTILITIES

std::string Client::getPrefix() const
{
	return (":" + this->_nickname + "!" + this->_username + "@" + this->_hostname);
}

std::string Client::getFullIdentifier() const
{
	return (this->_nickname + "!" + this->_username + "@" + this->_hostname);
}


//sendfile

#ifdef BONUS
// Guarda los datos base64 en bruto concatenados
void Client::handleFileChunk(const std::string& filename, const std::string& data)
{
    _receivedFiles[filename] += data;
}

// Decodifica el contenido base64 acumulado y lo guarda en disco
void Client::handleFileEnd(const std::string& filename)
{
    std::map<std::string, std::string>::iterator it = _receivedFiles.find(filename);
    if (it == _receivedFiles.end())
    {
        std::cerr << "[ERROR] No chunks received for file: " << filename << std::endl;
        return;
    }

    const std::string& base64 = it->second;
    std::string decoded = Base64::decodeBase64(base64);

    std::ofstream file(filename.c_str(), std::ios::binary);
    if (file)
    {
        file.write(decoded.c_str(), decoded.size());
        file.close();
        std::cout << "[RECEIVED] File saved: " << filename << std::endl;
    }
    else
    {
        std::cerr << "[ERROR] Failed to write file: " << filename << std::endl;
		this->sendMessage("NOTICE " + this->getNickName() + " : File '" + filename + "' received and saved successfully.");
    }

    _receivedFiles.erase(it);
}

#endif
