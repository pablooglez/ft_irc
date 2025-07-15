/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileManager.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 10:39:31 by albelope          #+#    #+#             */
/*   Updated: 2025/07/15 20:28:42 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <map>
#include <vector>
#include <string>
#include "FileTransfer.hpp"
#include "Base64.hpp"
#include "../include/Client.hpp"

class Server;

class FileManager
{
	private:
		std::map<std::string, FileTransfer*> _activeTransfers;

	public:
		FileManager();
		~FileManager();

		bool hasTransfer(const std::string &id) const;
		FileTransfer* getTransfer(const std::string &id);
		void addTransfer(const std::string &id, FileTransfer *transfer);
		void removeTransfer(const std::string &id);

		bool isFileTooLarge(const std::string& path);
		std::string readFileContent(const std::string& path);
		std::vector<std::string> splitIntoChunks(const std::string& input, size_t chunkSize);
		void sendFile(Server& server, Client& sender, const std::string& receiverNick, const std::string& filename);
};

#endif