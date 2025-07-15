/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileTransfer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 11:05:43 by albelope          #+#    #+#             */
/*   Updated: 2025/07/15 20:32:36 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileTransfer.hpp"


FileTransfer::FileTransfer(std::string filename, std::string sendernick,
					std::string recievernick, size_t totalsize) :
					_filename(filename), _senderNick(sendernick),
					_receiverNick(recievernick), _totalSize(totalsize),
					_receivedSize(0), _startTime(std::time(NULL)),
					_isDone(false) {
						
		_fileStream.open(filename.c_str(), std::ios::binary | std::ios::app);
	}

FileTransfer::~FileTransfer()
{
	if (_fileStream.is_open())
		_fileStream.close();
}


std::string FileTransfer::getFileName() const
{
	return _filename;
}

std::string FileTransfer::getSenderNick() const
{
	return _senderNick;
}

std::string FileTransfer::getReceiverNick() const
{
	return _receiverNick;
}

size_t FileTransfer::getTotalSize() const
{
	return _totalSize;
}

size_t FileTransfer::getReceivedSize() const
{
	return _receivedSize;
}


bool	FileTransfer::appendData(const std::string &data)
{
	if (!_fileStream.is_open())
		return false;

	_fileStream.write(data.c_str(), data.size());
	_receivedSize += data.size();

	if (_receivedSize > _totalSize)
		return false;
		
	if (_receivedSize == _totalSize)
		_isDone = true;

	return true;
}

bool	FileTransfer::isDone() const
{
	return _isDone;
}

bool	FileTransfer::isTimedOut(int maxSeconds) const
{
	return (std::time(NULL) - _startTime) > maxSeconds;
}




