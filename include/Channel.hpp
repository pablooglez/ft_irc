/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 12:36:00 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/07 16:26:05 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <set>
#include <string>
#include <vector>
#include <map>

class Server;
class Client;

class Channel
{
	private:
		// === CHANNEL IDENTIFICATION ===
		std::string					_name;							// Stores channel name (e.g., "#general", "#random") - Must start with # for public channels
																		// EJ: _name = "#programacion" permite a los usuarios unirse al canal con el comando JOIN
		std::string					_topic;							// Channel topic displayed to users when joining - Can be set by operators or anyone if not restricted
																		// EJ: _topic = "Bienvenidos a la discusión de programación!" se muestra al usar el comando TOPIC

		// === CHANNEL MODES AND ACCESS CONTROL ===
		std::string					_key;							// Password for protected channels (MODE +k) - Users must provide this to join
																		// EJ: _key = "secreto123" requiere JOIN #canal secreto123
		bool						_inviteOnly;					// Invite-only flag (MODE +i) - Only invited users can join when true
																		// EJ: _inviteOnly = true bloquea JOIN a menos que usuario esté en _invited
		bool						_topicRestricted;				// Topic protection flag (MODE +t) - Only operators can change topic when true
																		// EJ: _topicRestricted = true permite solo a ops usar comando TOPIC
		size_t						_userLimit;						// Maximum users allowed (MODE +l) - 0 or negative means no limit
																		// EJ: _userLimit = 50 bloquea JOIN cuando ya hay 50 usuarios presentes
		bool						_hasPassword;					// Flag indicating if password is set - Avoids confusion between empty string and no password
																		// EJ: _hasPassword = true indica que el canal tiene contraseña, incluso si _key está vacío. _hasPassword = false significa que no hay contraseña.
		bool						_hasUserLimit;					// Flag indicating if user limit is active - Distinguishes between no limit and limit of 0
																		// EJ: _hasUserLimit = true con _userLimit = 0 significa "nadie puede entrar"

		// === USER MANAGEMENT ===
		std::vector<Client*>		_members;						// List of all users in channel - Used for broadcasting messages and NAMES command
																		// EJ: _members = {cliente1, cliente2, cliente3} para broadcast de PRIVMSG
		std::set<Client*>			_operators;						// Channel operators with special privileges - Can kick, ban, change modes, set topic
																		// EJ: _operators = {cliente1} permite a cliente1 usar KICK, MODE, TOPIC
		std::set<Client*>			_invited;						// Users invited to join invite-only channel - Bypasses invite-only restriction
																		// EJ: _invited = {cliente2} permite a cliente2 hacer JOIN incluso con modo +i

	public:
		// === CONSTRUCTORS AND DESTRUCTOR ===
		Channel();													// Default constructor - Creates empty channel with default settings
																		// EJ: Channel ch; crea canal sin nombre, sin restricciones
		Channel(const std::string &name);							// Create channel with name only - Public channel with no restrictions
																		// EJ: Channel("#general") crea canal público que cualquiera puede unirse
		Channel(const std::string &name, const std::string &password);	// Create private channel with password - Automatically sets +k mode
																			// EJ: Channel("#privado", "secreto") crea canal protegido con contraseña
		Channel(const std::string &name, Client* creator);			// Create channel with creator as first operator - Used when user creates new channel
																		// EJ: Channel("#nuevochan", cliente) hace a cliente el primer operador
		Channel(const Channel &other);								// Copy constructor - Creates identical copy of existing channel
																		// EJ: Channel copia(original) duplica todos los miembros, modos, configuraciones
		Channel &operator=(const Channel &other);					// Assignment operator - Copies all data from another channel
																		// EJ: channel1 = channel2 copia todos los datos de channel2 a channel1
		~Channel();													// Destructor - Cleans up resources and notifies users of channel destruction
																		// EJ: delete channel envía mensajes PART a todos los miembros
		
		// === MEMBER MANAGEMENT ===
		
		bool						isMember(Client* member) const;					// Check if user is already in channel - Returns true if user is present
																					// EJ: if(isMember(client)) previene mensajes JOIN duplicados
		bool						addMember(Client* member);					// Add user to channel - Checks permissions, limits, passwords before adding
																					// EJ: addMember(client) devuelve false si canal es +i y client no está invitado
		bool						leaveChannel(Client* member);				// Remove user from channel - Also removes from operators/invited lists if present
																					// EJ: leaveChannel(client) en comando PART o QUIT
		bool						kickUser(Client* kicker, Client* target);	// Forcibly remove user from channel - Only operators can kick, sends KICK message
																					// EJ: kickUser(op, user) devuelve false si kicker no es operador
		bool						inviteUser(Client* inviter, Client* target);// Add user to invitation list - Allows joining invite-only channels
																					// EJ: inviteUser(op, user) permite a user hacer JOIN al canal +i

		// === OPERATOR MANAGEMENT ===
		bool						addOperator(Client* client);				// Grant operator privileges (MODE +o) - Gives kick, ban, mode change permissions
																					// EJ: addOperator(client) permite a client usar comandos KICK, MODE, TOPIC
		bool 						removeOperator(Client* client);				// Remove operator privileges (MODE -o) - Revokes special permissions
																					// EJ: removeOperator(client) impide a client usar comandos de operador
		bool						isOperator(Client* client) const;			// Check if user has operator privileges - Used before allowing privileged commands
																					// EJ: if(isOperator(client)) permite ejecución del comando KICK
		bool						isOperator(const std::string& nickname) const;// Check operator status by nickname - Useful when you only have the name
																					// EJ: isOperator("alice") verifica si usuario "alice" es operador
		
		// === INVITATION MANAGEMENT ===
		bool						addInvited(Client* client);				// Add user to invitation list - Allows bypassing invite-only restriction
																				// EJ: addInvited(client) al ejecutar comando INVITE
		bool						removeInvited(Client* client);			// Remove user from invitation list - Revokes invite-only bypass
																				// EJ: removeInvited(client) cuando usuario entra o sale del canal
		bool						isInvited(Client* client) const;		// Check if user is invited - Used in canJoin() validation
																				// EJ: if(isInvited(client)) permite JOIN al canal +i
		void						clearInvited();							// Clear all invitations - Useful when changing from +i to -i mode
																				// EJ: clearInvited() cuando MODE -i quita restricción de solo invitación

		// === CHANNEL QUERIES AND VALIDATION ===
		const std::vector<Client*>	&whoIsIn() const;						// Get reference to members list - Used for NAMES command and broadcasting
																			// EJ: for(Client* c : whoIsIn()) envía mensaje a cada miembro
		bool						canJoin(Client* client, const std::string& password = "") const;// Check if user can join channel - Validates password, limits, invites
																			// EJ: if(!canJoin(client, "pass")) envía error 475 (contraseña incorrecta)
		bool						channelIsEmpty() const;						// Check if channel has no members - Used to determine if channel should be deleted
																			// EJ: if(channelIsEmpty()) elimina canal del servidor
		bool						isEmpty() const;						// Alternative empty check - Const version for read-only operations
																			// EJ: if(isEmpty()) muestra "canal está vacío" en INFO
		bool						isFull() const;							// Check if channel reached user limit - Prevents joining when at capacity
																			// EJ: if(isFull()) envía error 471 (canal está lleno)
		int							getClientCount() const;					// Get current number of users - Used for user limit validation and INFO commands
																			// EJ: if(getClientCount() >= _userLimit) previene JOIN
		int							getOperatorCount() const;				// Get number of operators - Useful for ensuring at least one operator exists
																			// EJ: if(getOperatorCount() == 0) promueve a alguien a operador

		// === CLIENT SEARCH METHODS ===
		Client*						getClientbyNickname(const std::string& nickname)const;// Find user by nickname - Returns pointer to Client or nullptr if not found
																					// EJ: Client* user = getClient("alice"); if(user) envía mensaje privado
		bool						hasClient(const std::string& nickname)const;// Check if nickname exists in channel - Faster than getClient when you only need true/false
																					// EJ: if(hasClient("bob")) devuelve "usuario ya está en el canal"

		// === PERMISSION VALIDATION ===
		bool						canKick(Client* kicker, Client* target)const;// Check if kicker can kick target - Validates operator status and prevents self-kick
																					// EJ: if(!canKick(op, user)) envía error 482 (no es operador)
		bool						canSetTopic(Client* client) const;			// Check if user can change topic - Allows if not restricted or user is operator
																					// EJ: if(!canSetTopic(client)) envía error 482 (no puede cambiar topic)
		bool						canInvite(Client* client) const;			// Check if user can invite others - Usually allowed for all members or operators only
																					// EJ: if(!canInvite(client)) envía error 482 (no puede invitar)

		// === COMMUNICATION METHODS ===
		void						broadcast(const std::string& message, Client* sender = NULL) const;// Send message to all channel members - Used for JOIN, PART, MODE notifications
																											// EJ: broadcast(":alice!user@host JOIN #channel") notifica a todos los miembros
		void						broadcastToOthers(const std::string& message, Client* sender) const;// Send message to all except sender - Used for PRIVMSG, ACTION commands
																											// EJ: broadcastToOthers(":alice PRIVMSG #channel :hello", alice) envía a otros

		// === IRC PROTOCOL RESPONSES ===
		std::string					getNamesReply() const;					// Generate NAMES reply (353) - Lists all users with @+ prefixes for operators
																			// EJ: ":server 353 alice = #channel :@bob alice charlie" (bob es operador)
		std::string					getModes() const;						// Generate mode string like "+tnl 50" - Shows active channel modes with parameters
																			// EJ: "+tnl 50" significa topic-restricted, no external, límite 50 usuarios
		std::string					getTopicReply() const;					// Generate TOPIC reply (332) - Returns formatted topic response for clients
																			// EJ: ":server 332 alice #channel :¡Bienvenidos al canal!"
		std::string					getChannelInfo() const;					// Generate debug information - Complete channel state for troubleshooting
																			// EJ: "Channel #test: 5 usuarios, 2 ops, modos +tn, topic: Hola"
		std::string					getUserListString() const;				// Generate simple user list - Space-separated nicknames for logging
																			// EJ: "alice bob charlie dave eve" para archivos de log

		// === STATIC VALIDATION METHODS ===
		static bool					isValidChannelName(const std::string& name);// Validate channel name format - Must start with # and contain valid characters
																					// EJ: isValidChannelName("#test") = true, isValidChannelName("test") = false
		static bool					isValidPassword(const std::string& password);// Validate password format - Check length and forbidden characters
																					// EJ: isValidPassword("abc123") = true, isValidPassword("") = false
		static bool					isValidTopic(const std::string& topic);	// Validate topic content - Check length and forbidden characters
																					// EJ: isValidTopic("¡Bienvenidos!") = true, isValidTopic(cadena_muy_larga) = false

		// === GETTERS (READ-ONLY ACCESS) ===
		const std::string			&getChannelName() const;				// Get channel name - Returns reference to avoid copying
																			// EJ: std::string name = chan.getChannelName(); // name = "#general"
		const std::vector<Client*>	&getMembers() const;				// Get member list - Returns const reference for read-only access
																			// EJ: for(Client* c : chan.getMembers()) sendMessage(c, "hola");
		const std::string			&getPassword() const;				// Get channel password - Returns actual password string (be careful with access)
																			// EJ: if(userPassword != chan.getPassword()) rechaza_join();
		const std::string			&getTopic() const;					// Get channel topic - Current topic text visible to users
																			// EJ: send("332 alice #channel :" + chan.getTopic());
		bool						isInviteOnly() const;				// Check if invite-only mode is active - Returns true if +i mode is set
																			// EJ: if(chan.isInviteOnly() && !isInvited(user)) deniega_union();
		bool 						isTopicRestricted() const;			// Check if topic is operator-only - Returns true if +t mode is set
																			// EJ: if(chan.isTopicRestricted() && !isOp(user)) deniega_cambio_de_topic();
		int							getUserLimit() const;				// Get user limit value - Returns limit number, 0 or negative means no limit
																			// EJ: if(chan.getUserLimit() > 0 && count >= limit) deniega_union();
		bool						hasPassword() const;				// Check if password is set - True if channel requires password to join
																			// EJ: if(chan.hasPassword()) requiere_contraseña_para_union();
		bool						hasUserLimit() const;				// Check if user limit is active - True if channel has maximum user restriction
																			// EJ: if(chan.hasUserLimit()) verifica_cantidad_usuarios_antes_union();
		
		// === SETTERS (MODIFY CHANNEL STATE) ===
		void						setPassword(const std::string &password);// Set channel password - Enables +k mode and password protection
																				// EJ: chan.setPassword("secreto123"); // MODE +k secreto123
		void						setTopic(const std::string &topicName);	// Change channel topic - Updates topic text visible to all users
																				// EJ: chan.setTopic("¡Bienvenidos a programación!"); // comando TOPIC
		void						setChannelName(const std::string &name);	// Change channel name - Rare operation, updates channel identifier
																				// EJ: chan.setChannelName("#nuevonombre"); // renombra canal
		void						setInviteOnly(bool invite);				// Toggle invite-only mode - Enables/disables +i mode restriction
																				// EJ: chan.setInviteOnly(true); // MODE +i
		void						setTopicRestricted(bool restricted);	// Toggle topic restriction - Enables/disables +t mode (operator-only topic)
																				// EJ: chan.setTopicRestricted(true); // MODE +t
		bool						setUserLimit(int limit);			// Set user limit - Enables +l mode with specified maximum users, returns success
																			// EJ: if(chan.setUserLimit(50)) send("MODE +l 50"); else send_error();
													
		
		//=====================================================================================
		// === FUTURES (CHECK IF WE NEEDED AND WE ARE NOT TIRED OF FT_IRC)=====
				//INFO TIMESTAMPS-----
		//time_t							_creationTime;					// Channel creation timestamp - Used for channel age and INFO responses
                                                               				 // EJ: _creationTime = time(NULL) guarda cuándo se creó el canal
		//time_t							_topicTime;						// Topic change timestamp - Shows when topic was last modified
                                                               				 // EJ: _topicTime = time(NULL) actualizado cuando se cambia el topic
		//std::string						_topicSetBy;					// Who set the current topic - Shows nickname of user who set topic
                                                                			// EJ: _topicSetBy = "alice" muestra quién estableció el topic actual
				//FLOOD PROTECTION ANTI SPAM MESSAGES---
		//int								_messageRateLimit;				// Messages per second limit - Prevents spam/flooding
                                                                			// EJ: _messageRateLimit = 5 permite máximo 5 mensajes por segundo
		//std::map<Client*, time_t>		_lastMessageTime;				// Last message time per user - Used for rate limiting
                                                                			// EJ: _lastMessageTime[cliente] = time(NULL) para control de spam

				//CHEK BAN OPTIONS LATER	
				
				


		//PABLO FUN 

		bool							removeClient(Client *client);
};