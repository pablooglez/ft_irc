/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileRebuilder.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 12:28:20 by albelope          #+#    #+#             */
/*   Updated: 2025/07/15 20:30:04 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileRebuilder.hpp"
#include "../include/Server.hpp"
#include <iostream>

FileRebuilder::FileRebuilder()
{
}

FileRebuilder::~FileRebuilder()
{
	_bufferMap.clear(); 
}

std::string FileRebuilder::buildKey(const std::string& nick, const std::string& filename) const
{
	return nick + ":" + filename;
}

void FileRebuilder::writeFile(const std::string& filename, const std::string& content)
{
	std::ofstream outfile(filename.c_str(), std::ios::binary);
	if (!outfile)
		return;
	outfile.write(content.c_str(), content.size());
	outfile.close();
}

void FileRebuilder::handleChunk(Client* client, const std::string& command, const std::string& filename, const std::string& data)
{
	(void)command;

	if (!client)
		return;

	std::string key = buildKey(client->getNickName(), filename);
	_bufferMap[key].filename = filename;
	_bufferMap[key].chunks.push_back(data);
}

void FileRebuilder::handleEnd(Client* client, const std::string& filename)
{
	if (!client)
		return;

	std::string key = buildKey(client->getNickName(), filename);
	if (_bufferMap.find(key) == _bufferMap.end())
		return;

	std::string encoded;
	for (size_t i = 0; i < _bufferMap[key].chunks.size(); ++i)
		encoded += _bufferMap[key].chunks[i];

	std::string decoded = Base64::decodeBase64(encoded);
	writeFile(filename, decoded);
	client->sendMessage("NOTICE :File " + filename + " saved successfully\r\n");
	_bufferMap.erase(key);
}