/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileTransfer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 11:05:43 by albelope          #+#    #+#             */
/*   Updated: 2025/07/07 13:59:17 by albelope         ###   ########.fr       */
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
		//Obligatorio cuando manejas archivos que pueden contener datos no-texto (imágenes, ejecutables, cualquier archivo binario).
		//in esto, el sistema puede interpretar bytes de forma incorrecta, sobre todo los \n y \r.
		// Significa "append" (añadir al final) y es la mejor opción para transferencias por chunks.
 		//garantizas que Cada vez que escribes un nuevo chunk, se añade al final.
		//No sobrescribes los datos anteriores.		
	}

FileTransfer::~FileTransfer() {
	if (_fileStream.is_open())
		_fileStream.close();
}


std::string FileTransfer::getFileName() const {
	return _filename;
}

std::string FileTransfer::getSenderNick() const {
	return _senderNick;
}

std::string FileTransfer::getReceiverNick() const {
	return _receiverNick;
}

size_t FileTransfer::getTotalSize() const {
	return _totalSize;
}

size_t FileTransfer::getReceivedSize() const {
	return _receivedSize;
}


bool	FileTransfer::appendData(const std::string &data) {
	// si el archivo no esta abierto, fallo
	if (!_fileStream.is_open())
		return false;

	// escribe los datos en el archivo binario
	_fileStream.write(data.c_str(), data.size());
	// suma el tamaño recibido
	_receivedSize += data.size();

	// si se pasa del tamaño que deberia, fallo
	if (_receivedSize > _totalSize)
		return false;

	// si he recibido justo todo, marco que esta hecho
	if (_receivedSize == _totalSize)
		_isDone = true;

	return true;
}

bool	FileTransfer::isDone() const {
	return _isDone;
}

bool	FileTransfer::isTimedOut(int maxSeconds) const {
		// si el tiempo actual menos cuando empezo es mayor que el maximo, devuelve true (timeout)
	return (std::time(NULL) - _startTime) > maxSeconds;
}




