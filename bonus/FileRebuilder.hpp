/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileRebuilder.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 12:28:17 by albelope          #+#    #+#             */
/*   Updated: 2025/07/15 20:31:10 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEREBUILDER_HPP
#define FILEREBUILDER_HPP

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "Base64.hpp"
#include "../include/Client.hpp"

class Server;

class FileRebuilder
{
	private:
		struct FileBuffer
		{
			std::vector<std::string> chunks;
			std::string filename;
		};

	std::map<std::string, FileBuffer> _bufferMap;

	std::string buildKey(const std::string& nick, const std::string& filename) const;
	void writeFile(const std::string& filename, const std::string& content);

	public:
		FileRebuilder();
		~FileRebuilder();

		void handleChunk(Client* client, const std::string& command, const std::string& filename, const std::string& data);
		void handleEnd(Client* client, const std::string& filename);
};

#endif