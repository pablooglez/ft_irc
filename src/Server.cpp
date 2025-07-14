/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 17:05:24 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/14 19:38:58 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include <algorithm>
#include <cctype>
#include "../include/RPL.hpp"
#ifdef BONUS
# include "../bonus/FileManager.hpp"
# include "../bonus/FileTransfer.hpp"
# include "../bonus/Base64.hpp"
#endif

Server::Server(int port, const std::string &password)
{
	_port = port;
	_password = password;
	_server_name = "localhost";
	_server_fd = -1;
	_running = false;
}

Server::~Server()
{
	cleanup();
}

int	Server::GetPort() const
{
	return this->_port;
}

const std::string	&Server::GetPassword() const
{
	return this->_password;
}

const std::string& Server::getServerName() const
{
	return this->_server_name;
}

int	Server::getServerFd() const
{
	return this->_server_fd;
}

std::string Server::getClientNick(int client_fd) const
{
	std::map<int, Client>::const_iterator it = _clients.find(client_fd);
	if (it != _clients.end() && !it->second.getNickName().empty())
		return it->second.getNickName();
	return "*";	// Client without nicknname
}

bool	Server::setupSocket()
{
	std::cout << "....Configure setupSocket...." << std::endl;

	_server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (_server_fd == -1)
	{
		std::cerr << "Error: socket() failed" << std::endl;
		return (false);
	}

	int	opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error: setsockopt() failed" << std::endl;
		return (false);
	}

	std::memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	_server_addr.sin_port = htons(_port);

	if (bind(_server_fd, (struct sockaddr*) &_server_addr, sizeof(_server_addr)) < 0)
	{
		std::cerr << "Error: bind() failed" << std::endl;
		cleanup();
		return (false);
	}

	if (listen(_server_fd, SOMAXCONN) < 0)
	{
		std::cerr << "Error: listen() failed" << std::endl;
		cleanup();
		return (false);
	}

	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: fcntl() failed" << std::endl;
		cleanup();
		return (false);
	}

	struct pollfd server_pollfd;
	server_pollfd.fd = _server_fd;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;

	_poll_fds.push_back(server_pollfd);

	
	std::cout << "Server socket created succesfully" << std::endl;
	std::cout << "Listening in port: " << _port << std::endl;
	std::cout << "Password: " << _password << std::endl;

	return (true);
}

void	Server::start()
{
	if (!setupSocket())
	{
		std::cerr << "Error: Failed to setup server socket" << std::endl;
		return;
	}

	_running = true;

	std::cout << "    SERVER STARTED    " << std::endl;

	runServerLoop();
}

void	Server::stop()
{
	_running = false;

	cleanup();
}

void	Server::cleanup()
{
	for (size_t i = 0; i < _client_fds.size(); i++)
	{
		close(_client_fds[i]);
	}
	_client_fds.clear();

	_poll_fds.clear();
	
	_client_buffers.clear();
	_clients.clear();

	if (_server_fd != -1)
	{
		close(_server_fd);
		_server_fd = -1;
	}
	
	std::cout << "Server cleanup completed" << std::endl;
}

void	Server::handleSignal(int signal)
{
	g_global = signal;
}

void	Server::runServerLoop()
{
	signal(SIGINT, Server::handleSignal);

	while (this->_running)
	{
		if (g_global == SIGINT)
		{
			this->_running = false;
			break;
		}

		int	poll_result = poll(&this->_poll_fds[0], this->_poll_fds.size(), -1);

		if (poll_result == -1 && g_global == 0)
		{
			std::cerr << "Error: poll() failed" << std::endl;
			break;
		}
		if (poll_result == 0)
		{
			continue;
		}

		for (size_t i = 0; i < this->_poll_fds.size(); i++)
		{
			if (this->_poll_fds[i].revents & POLLIN)
			{
				if (this->_poll_fds[i].fd == this->_server_fd)
					handleNewConnection();
				else
					handleClientData(this->_poll_fds[i].fd);
			}
			if (this->_poll_fds[i].revents & (POLLHUP | POLLERR))
			{
				// Handle client disconnection
				if (this->_poll_fds[i].fd != this->_server_fd)
				{
					std::cout << "Error: Client disconnected (error/hangup)" << std::endl;
					removeClient(this->_poll_fds[i].fd);
					i--;	// Adjust index because we removed an element
				}
			}
		}
	}
	std::cout << "    SERVER STOPPED    " << std::endl;
	cleanup();
}

void	Server::handleNewConnection()
{
	std::cout << "...Handling new connection..." << std::endl;

	struct sockaddr_in client_addr;	// Structure for handling customer information
	socklen_t client_len = sizeof(client_addr);

	//Accept() accept pending connection
	int	client_fd = accept(this->_server_fd, (struct sockaddr*) &client_addr, &client_len);
	
	if (client_fd == -1)
	{
		std::cerr << "Error: accept() failed" << std::endl;
		return;
	}

	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)	// Socket Not-blocking
	{
		std::cerr << "Error: fcntl() failed for client socket" << std::endl;
		close(client_fd);
		return;
	}
	
	struct pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;
	
	_poll_fds.push_back(client_pollfd);
	_client_fds.push_back(client_fd);
	
	char	client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
	
	createClientObject(client_fd, std::string(client_ip));
	
	std::cout << "New client connected:" << std::endl;
	std::cout << "  - FD: " << client_fd << std::endl;
	std::cout << "  - IP: " << client_ip << std::endl;
	std::cout << "  - Port: " << ntohs(client_addr.sin_port) << std::endl;
	std::cout << "  - Client object created" << std::endl;
	std::cout << "  - Total clients: " << _client_fds.size() << std::endl;
}

void	Server::handleClientData(int client_fd)
{
	std::cout << "...Handling data from client (FD: " << client_fd << ")..." << std::endl;

	char buffer[1024];

	ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_received < 0)
	{
		std::cerr << "Error: recv() failed for client " << client_fd << std::endl;
		return;
	}
	else if (bytes_received == 0)
	{
		std::cout << "Client (FD: " << client_fd << ") disconnected" << std::endl;
		removeClient(client_fd);
		return;
	}

	buffer[bytes_received] = '\0';	// Finish buffer of null

	_client_buffers[client_fd] += std::string(buffer, bytes_received);	// Add the new data to the client buffer
	
	processClientMessages(client_fd); // Process Complet messages
}

void	Server::removeClient(int client_fd)
{
	std::cout << "...Removing client (FD: " << client_fd << ")..." << std::endl;
	
	std::vector<int>::iterator it = std::find(_client_fds.begin(), _client_fds.end(), client_fd);
	
	if (it != _client_fds.end())
	{
		_client_fds.erase(it);
		std::cout << "Client removed from _client_fds" << std::endl;
	}
	else
		std::cerr << "Error: Client FD not found in _client_fds" << std::endl;
	
	for (std::vector<struct pollfd>::iterator poll_it = _poll_fds.begin(); poll_it != _poll_fds.end(); ++poll_it)
	{
		if (poll_it->fd == client_fd)
		{
			_poll_fds.erase(poll_it);
			std::cout << "Client removed from _poll_fds" << std::endl;
			break;
		}
	}

	// Remove client buffer
	_client_buffers.erase(client_fd);
	std::cout << "Client buffer removed" << std::endl;

	// Remove client object
	_clients.erase(client_fd);
	std::cout << "Client object removed" << std::endl;

	if (close(client_fd) == -1)
		std::cerr << "Error: Failed to close client socket" << std::endl;
	else
		std::cout << "Client socket closed successfully" << std::endl;

	std::cout << "Client cleanup completed. Remaining clients: " << this->_client_fds.size() << std::endl;
}

int	Server::findClientIndex(int client_fd)
{
	for (size_t i = 0; i < this->_client_fds.size(); i++) // Search in _client_fds
	{
		if (this->_client_fds[i] == client_fd)
			return static_cast<int>(i);
	}
	return (-1);
}

void	Server::processClientMessages(int client_fd)
{
	std::string &buffer = _client_buffers[client_fd];
	size_t pos = 0;

	// Search messages ending in \r\n or \n
	while ((pos = buffer.find("\r\n")) != std::string::npos || (pos = buffer.find("\n")) != std::string::npos)
	{
		std::string message = buffer.substr(0, pos);

		if (buffer.substr(pos, 2) == "\r\n")	// Determine it is \r\n or \n to know how many characters to delete.
		{
			buffer.erase(0, pos + 2);			// Eliminate message + \r\n
		}
		else
		{
			buffer.erase(0, pos + 1);			// Eliminate message + \n
		}

		if (!message.empty())
		{
			std::cout << "Message received from FD" << client_fd << ": " << message << std::endl;
			parceIRCMessage(client_fd, message, ' '); // Usar espacio como delimitador para comandos IRC
		}
	}
}

void	Server::parceIRCMessage(int client_fd, const std::string &message, char delimiter)
{
	std::vector<std::string> tokens = splitMessage(message, delimiter);

	if (tokens.empty())
	{
		return;
	}

	std::string command = tokens[0];
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);
	std::cout << "Command: " << command << std::endl;

	Client *client = findClientByFd(client_fd);
	if (!client)
	{
		std::cerr << "Error: Client not found for fd " << client_fd << std::endl;
		return;
	}

	// Commands allowed during registration process
	if (command == "PASS")
		PassCommand(client_fd, tokens);
	else if (command == "NICK")
		NickCommand(client_fd, tokens);
	else if (command == "USER")
		UserCommand(client_fd, tokens);
	else if (command == "QUIT")
		QuitCommand(client_fd, tokens);
	
	// Commands that require full registration
	else if (!client->isRegistered())
	{
		// Send standard IRC error code without custom messages
		std::string error = ":localhost 451 * :You have not registered\r\n";
		client->sendMessage(error);
		return;
	}
	else if (command == "JOIN")
		JoinCommand(client_fd, tokens);
	else if (command == "PART")
		PartCommand(client_fd, tokens);
	else if (command == "KICK")
		KickCommand(client_fd, tokens);
	else if (command == "INFO")
		InfoCommand(client_fd, tokens);
	else if (command == "TOPIC")
		TopicCommand(client_fd, tokens);
	else if (command == "PRIVMSG")
		PrivmsgCommand(client_fd, tokens);
	else if (command == "INVITE")
		InviteCommand(client_fd, tokens);
	else if (command == "LIST")
		ListCommand(client_fd, tokens);
	else if (command == "NAMES")
		NamesCommand(client_fd, tokens);
	else if (command == "MODE")
		ModesCommand(client_fd, tokens);
	#ifdef BONUS
	else if (command == "SENDFILE")
		SendFileCommand(client_fd, tokens);
	#endif
}


// CLIENT MANAGEMENT FUNCTIONS

void Server::createClientObject(int client_fd, const std::string &hostname)
{
	Client new_client(client_fd, hostname);
	_clients[client_fd] = new_client;

	std::cout << "Client object created for FD: " << client_fd << " with hostname: " << hostname << std::endl;

}

Client* Server::findClientByFd(int client_fd)
{
	std::map<int, Client>::iterator it = _clients.find(client_fd);
	if (it != _clients.end())
	return &(it->second);
	return NULL;
}

bool Server::isNicknameInUse(const std::string &nickname)
{
	if (nickname.empty())
	return false;
	
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickName() == nickname)
		return true;
	}
	return false;
}

bool Server::isValidNickname(const std::string &nickname)
{
	if (nickname.empty() || nickname.length() > 9)	// Check if nickname is empty or too long (RFC 2812: max 9 characters)
	return false;

	char first = nickname[0];	// First character must be a letter or special character (not a digit)
	
	if (!std::isalpha(first) && first != '_' && first != '[' && first != ']' && first != '\\' && first != '`' && first != '^' && first != '{' && first != '}')
	return false;
	
	for (size_t i = 0; i < nickname.length(); ++i)	// Check all characters are valid
	{
		char c = nickname[i];
		if (!std::isalnum(c) && c != '_' && c != '-' && c != '[' && c != ']' && c != '\\' && c != '`' && c != '^' && c != '{' && c != '}')
		return false;
	}
	return true;
}

// WELCOME MESSAGES
void	Server::sendWelcomeMessages(int client_fd)
{
	Client *client = findClientByFd(client_fd);
	if (!client)
		return;

	// RPL_WELCOME (001)
	std::string welcome = RPL::RPL_WELCOME(getServerName(), client->getNickName(), client->getUserName(), client->getHostName());
	client->sendMessage(welcome);

	// RPL_YOURHOST (002)
	std::string yourhost = RPL::RPL_YOURHOST(getServerName(), client->getNickName(), getServerName(), "1.0");
	client->sendMessage(yourhost);

	// RPL_CREATED (003)
	std::string created = RPL::RPL_CREATED(getServerName(), client->getNickName(), "June 20, 2025");
	client->sendMessage(created);

	// RPL_MYINFO (004)
	std::string myinfo = RPL::RPL_MYINFO(getServerName(), client->getNickName(), getServerName(), "1.0", "i", "o");
	client->sendMessage(myinfo);
	
	std::cout << "Welcome messages sent to client " << client_fd << " (" << client->getNickName() << ")" << std::endl;
}

// CHANNEL MANAGEMENT FUNCTIONS

Client* Server::findClientByNickname(const std::string& nickname)
{
	if (nickname.empty())
		return NULL;
		
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickName() == nickname)
			return &(it->second);
	}
	return NULL;
}


Channel* Server::findOrCreateChannel(const std::string &channel_name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
	if (it != _channels.end())
	{
		return &(it->second);
	}
	
	// Create new channel
	Channel new_channel(channel_name);
	_channels[channel_name] = new_channel;
	std::cout << "New channel created: " << channel_name << std::endl;
	
	return &_channels[channel_name];
}

Channel* Server::findChannel(const std::string &channel_name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
	if (it != _channels.end())
		return &(it->second);
	return NULL;
}

bool Server::channelExists(const std::string &channel_name)
{
	return _channels.find(channel_name) != _channels.end();
}

void Server::removeChannelIfEmpty(const std::string &channel_name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channel_name);
	if (it != _channels.end() && it->second.isEmpty())
	{
		_channels.erase(it);
		std::cout << "Empty channel removed: " << channel_name << std::endl;
	}
}




//SENDFILES
#ifdef BONUS
void Server::SendFileCommand(int client_fd, const std::vector<std::string> &tokens)
{
	if (tokens.size() < 3)
	{
		Client* client = findClientByFd(client_fd);
		if (client)
			client->sendMessage("ERROR :Usage: SENDFILE <receiver_nick> <filename>\r\n");
		return;
	}

	std::string receiver_nick = tokens[1];
	std::string filename = tokens[2];

	Client* sender = findClientByFd(client_fd);
	Client* receiver = findClientByNickname(receiver_nick);

	if (!sender || !receiver)
	{
		if (sender)
			sender->sendMessage("ERROR :Invalid sender or receiver\r\n");
		return;
	}

	// Read file of hardisk
	std::ifstream file(filename.c_str(), std::ios::binary);
	if (!file)
	{
		sender->sendMessage("ERROR :File not found\r\n");
		return;
	}

	// read content
	std::ostringstream buffer;
	buffer << file.rdbuf();
	std::string fileContent = buffer.str();
	file.close();

	// cofigy to 64
	std::string base64 = Base64::encodeBase64(fileContent);

	size_t chunkSize = 400; // size for chunk
	size_t totalChunks = (base64.size() + chunkSize - 1) / chunkSize;

	// Send info
	receiver->sendMessage("NOTICE :" + sender->getNickName() + " is sending you a file: " + filename + "\r\n");

	// Send privmsg like chunk
	for (size_t i = 0; i < totalChunks; ++i)
	{
		std::string chunk = base64.substr(i * chunkSize, chunkSize);
		receiver->sendMessage("PRIVMSG " + receiver_nick + " :FILECHUNK " + filename + " " + chunk + "\r\n");
		usleep(30000); // anti saturation
	}

	// Final send
	receiver->sendMessage("PRIVMSG " + receiver_nick + " :FILEEND " + filename + "\r\n");
	sender->sendMessage("NOTICE :File sent successfully\r\n");
}
#endif