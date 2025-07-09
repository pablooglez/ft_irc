/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileTransfer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 11:05:40 by albelope          #+#    #+#             */
/*   Updated: 2025/07/07 13:59:18 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef FILETRANSFER_HPP
#define FILETRANSFER_HPP

#include <iostream>
#include <fstream>
#include <ctime>

class FileTransfer {
		private:
			std::string		_filename;
			std::string		_senderNick;
			std::string		_receiverNick;
			size_t			_totalSize;
			size_t			_receivedSize;
			std::ofstream	_fileStream;
			std::time_t		_startTime;
			bool			_isDone;
			
		public:
			FileTransfer(std::string filename, std::string sendernick, std::string recievernick, size_t totalsize);
			~FileTransfer();

			std::string getFileName() const;
			std::string getSenderNick() const;
			std::string getReceiverNick() const;
			size_t		getTotalSize() const;
			size_t		getReceivedSize() const;

			bool		appendData(const std::string &data);
			bool		isDone() const;
			bool		isTimedOut(int maxSeconds) const;

		
};

#endif

