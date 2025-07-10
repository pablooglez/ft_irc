/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 20:59:11 by albelope          #+#    #+#             */
/*   Updated: 2025/07/10 19:22:41 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/BotCmd.hpp"
#include "include/Bot.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>



Bot::Bot(const std::string& ip, int port, const std::string& pass, const std::string& channel)
{
	this->_ip = ip;
	this->_port = port;
	this->_pass = pass;
	this->_nick = "Bot";
	this->_user = "Bot";
	this->_fd = -1;

	if (channel[0] != '#')
		_channel = "#" + channel;
	else
		_channel = channel;
}

Bot::~Bot()
{
	if (_fd != -1)
		close(_fd);
}

std::string Bot::getChannel() const
{
	return _channel;
}

bool Bot::safeSend(const std::string& msg)
{
	ssize_t total = 0;
	while (total < (ssize_t)msg.size())
	{
		ssize_t n = send(_fd, msg.c_str() + total, msg.size() - total, 0);
		if (n <= 0)
			return false;
		total += n;
	}
	return true;
}

void Bot::start()
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (_fd < 0)
	{
		std::cerr << "Error: socket gone wrong" << std::endl;
		exit(1);
	}
	std::cout << "Socket created for bot" << std::endl;

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(_port);

	if (inet_pton(AF_INET, _ip.c_str(), &serverAddr.sin_addr) <= 0)
	{
		std::cerr << "IP Address invalid" << std::endl;
		exit(1);
	}

	if (connect(_fd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Error: connect() failed" << std::endl;
		exit(1);
	}
	std::cout << "Bot connected successfully!" << std::endl;

	std::string passCmd = "PASS " + _pass + "\r\n";
	send(_fd, passCmd.c_str(), passCmd.length(), 0);

	std::string nickCmd = "NICK " + _nick + "\r\n";
	send(_fd, nickCmd.c_str(), nickCmd.length(), 0);

	std::string userCmd = "USER " + _user + " 0 * :" + _user + "\r\n";
	send(_fd, userCmd.c_str(), userCmd.length(), 0);

	std::string joinCmd = "JOIN " + _channel + "\r\n";
	send(_fd, joinCmd.c_str(), joinCmd.length(), 0);

	char buffer[512];

	while (true)
	{
		ssize_t n = recv(_fd, buffer, sizeof(buffer) - 1, 0);
		if (n <= 0)
		{
			std::cerr << "server closed\n";
			break;
		}

		buffer[n] = '\0';
		_bufferBot.append(buffer);

		size_t pos;
		while ((pos = _bufferBot.find("\r\n")) != std::string::npos)
		{
			std::string line = _bufferBot.substr(0, pos);
			_bufferBot.erase(0, pos + 2);
			std::cout << ">> " << line << '\n';

			if (handlePing(line))
				continue;

			processMessage(line);
		}
	}
}

bool Bot::handlePing(const std::string& line)
{
	size_t p = line.find("PING");

	if (p == std::string::npos)
		return false;

	std::string payload = line.substr(line.find(":"));
	
	safeSend("PONG " + payload + "\r\n");

	return true;
}

void Bot::processMessage(const std::string& line)
{
	if (line.find("PRIVMSG " + _channel) == std::string::npos)
		return;

	size_t msgPos = line.find(" :");
	if (msgPos == std::string::npos)
		return;

	std::string text = line.substr(msgPos + 2);

	handleCommands(this, text);
}
