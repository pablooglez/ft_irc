/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileManager.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 10:39:43 by albelope          #+#    #+#             */
/*   Updated: 2025/07/10 12:42:04 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileManager.hpp"
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <vector>
#include "../include/Client.hpp"
#include "Base64.hpp"
#include "../include/Server.hpp"


#define MAX_FILE_SIZE_BYTES 5242880 // 5MB
#define CHUNK_SIZE 512 // Tamaño base de 64 por contenedor

FileManager::FileManager() {}

FileManager::~FileManager() {
	for (std::map<std::string, FileTransfer*>::iterator it = _activeTransfers.begin(); 
         it != _activeTransfers.end(); ++it) {
        delete it->second;
        it->second = NULL;
    }
	_activeTransfers.clear();
}

bool FileManager::hasTransfer(const std::string &id) const {
	return _activeTransfers.find(id) != _activeTransfers.end();
}

FileTransfer* FileManager::getTransfer(const std::string &id) {
	if (hasTransfer(id))
		return _activeTransfers[id];
	return NULL;
}

void FileManager::addTransfer(const std::string &id, FileTransfer *transfer) {
	_activeTransfers[id] = transfer;
}

void FileManager::removeTransfer(const std::string &id) {
	std::map<std::string, FileTransfer*>::iterator it = _activeTransfers.find(id);
	if (it != _activeTransfers.end())
	{
		delete it->second;
		_activeTransfers.erase(it);
	}
}

//


bool FileManager::isFileTooLarge(const std::string& path)
{
    struct stat fileStat;
    return (stat(path.c_str(), &fileStat) != 0 || fileStat.st_size > MAX_FILE_SIZE_BYTES);
}

std::string FileManager::readFileContent(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file)
        return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<std::string> FileManager::splitIntoChunks(const std::string& input, size_t chunkSize)
{
    std::vector<std::string> chunks;
    for (size_t i = 0; i < input.length(); i += chunkSize)
        chunks.push_back(input.substr(i, chunkSize));
    return chunks;
}


void FileManager::sendFile(Server& server, Client& sender, const std::string& receiverNick, const std::string& filename) {
	if (receiverNick.empty() || filename.empty()) {
		sender.sendMessage("ERROR :Missing receiver or filename");
		return;
	}
	
	std::ifstream file_test(filename.c_str(), std::ios::in | std::ios::binary);
	if (!file_test.is_open()) {
		sender.sendMessage("ERROR :File not found. Please check the filename.");
		return;
	}
	file_test.close();

	
	if (isFileTooLarge(filename)) {
		sender.sendMessage("ERROR :File is too large (max 5MB)");
		return;
	}

	std::string content = readFileContent(filename);
	if (content.empty()) {
		sender.sendMessage("ERROR :File not found or empty");
		return;
	}

	Client* receiver = server.findClientByNickname(receiverNick);
	if (!receiver) {
		sender.sendMessage("ERROR :Receiver not found");
		return;
	}

	if (receiver == &sender) {
		sender.sendMessage("ERROR :Cannot send a file to yourself");
		return;
	}

	std::string encoded = Base64::encodeBase64(content);
	std::vector<std::string> chunks = splitIntoChunks(encoded, CHUNK_SIZE);

	receiver->sendMessage("NOTICE :" + sender.getNickName() + " is sending you a file: " + filename);

	for (size_t i = 0; i < chunks.size(); ++i) {
		std::ostringstream oss;
		oss << "NOTICE " << receiverNick << " :\001FILECHUNK " << filename << " " << chunks[i] << "\001";
		receiver->sendMessage(oss.str());
		usleep(30000); // anti-flood
	}

	receiver->sendMessage("NOTICE " + receiverNick + " :\001FILEEND " + filename + "\001");
	sender.sendMessage("NOTICE :File sent successfully");
}