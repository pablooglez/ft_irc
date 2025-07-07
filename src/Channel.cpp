/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 16:23:01 by pablogon          #+#    #+#             */
/*   Updated: 2025/07/07 16:30:06 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include <sstream>

// === CONSTRUCTORS ===

Channel::Channel()
	: _name(""), _topic(""), _key(""), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(false),
	_hasUserLimit(false) {}
/*
 * EXPLICACIÓN Constructor por defecto:
 * - _name("") = Canal sin nombre específico, se asignará después
 * - _topic("") = Sin topic inicial, los usuarios pueden establecer uno
 * - _key("") = Sin contraseña, cualquiera puede entrar libremente
 * - _inviteOnly(false) = No requiere invitación, es un canal público
 * - _topicRestricted(false) = Cualquier usuario puede cambiar el topic
 * - _userLimit(0) = Sin límite de usuarios, valor 0 significa "sin restricción"
 * - _hasPassword(false) = Indica que no hay contraseña configurada
 * - _hasUserLimit(false) = Indica que no hay límite de usuarios activo
 * 
 * ¿Por qué estos valores? Creamos un canal completamente abierto y sin restricciones
 * como estado inicial más permisivo, después se pueden añadir restricciones.
 */

Channel::Channel(const std::string &name)
	: _name(name), _topic(""), _key(""), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(false),
	_hasUserLimit(false) {}
/*
 * EXPLICACIÓN Constructor con nombre:
 * - _name(name) = Asignamos el nombre proporcionado al canal
 * - Resto igual al constructor por defecto = Canal público sin restricciones
 * 
 * ¿Por qué? Este es el caso más común: crear un canal público como "#general" 
 * donde cualquiera puede entrar. Es el comportamiento estándar de IRC cuando 
 * alguien hace "JOIN #nombrecanal" sin contraseña.
 */

Channel::Channel(const std::string &name, const std::string &password)
	: _name(name), _topic(""), _key(password), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(!password.empty()),
	_hasUserLimit(false) {}
/*
 * EXPLICACIÓN Constructor con contraseña:
 * - _name(name) = Nombre del canal proporcionado
 * - _key(password) = Guardamos la contraseña proporcionada
 * - _hasPassword(!password.empty()) = Si password no está vacío, hay contraseña
 * - Resto sin restricciones adicionales
 * 
 * ¿Por qué !password.empty()? Si el string no está vacío, significa que SÍ hay 
 * contraseña. Este boolean nos ayuda a saber rápidamente si el canal requiere 
 * contraseña sin tener que verificar si _key está vacío cada vez.
 * 
 * Ejemplo: Channel("#privado", "secreto123") crea canal protegido.
 */

Channel::Channel(const std::string &name, Client* creator)
	: _name(name), _topic(""), _key(""), _inviteOnly(false),
	_topicRestricted(false), _userLimit(0), _hasPassword(false),
	_hasUserLimit(false)
{
	if (creator)
	{
		_members.push_back(creator);
		_operators.insert(creator);
	}
}
/*
 * EXPLICACIÓN Constructor con creador:
 * - Inicialización igual que constructor con nombre (canal público)
 * - if (creator) = Verificamos que el puntero no sea NULL/nullptr
 * - _members.push_back(creator) = Añadimos al creador como miembro del canal
 * - _operators.insert(creator) = Damos privilegios de operador al creador
 * 
 * ¿Por qué esta lógica? En IRC, cuando alguien crea un canal nuevo, automáticamente:
 * 1. Se convierte en el primer miembro
 * 2. Obtiene privilegios de operador (@)
 * 3. Puede gestionar el canal (kick, ban, cambiar modos)
 * 
 * ¿Por qué verificar if(creator)? Seguridad - evitamos crashes si alguien 
 * pasa un puntero nulo accidentalmente.
 * 
 * Ejemplo: Cuando usuario "alice" hace JOIN #nuevochannel y el canal no existe,
 * se crea automáticamente y alice se convierte en operador.
 */

Channel::Channel(const Channel &other) 
 	: _name(other._name),  _topic(other._topic), _key(other._key),
	_inviteOnly(other._inviteOnly), _topicRestricted(other._topicRestricted),
	_userLimit(other._userLimit), _hasPassword(other._hasPassword),
	_hasUserLimit(other._hasUserLimit) {}
/*
 * EXPLICACIÓN Constructor de copia:
 * - Copiamos SOLO la configuración del canal, NO los usuarios
 * - _name, _topic, _key = Información básica del canal
 * - Flags booleanos = Modos y restricciones (_inviteOnly, _topicRestricted, etc.)
 * - _userLimit = Límite de usuarios configurado
 * 
 * ¿Por qué NO copiamos _members, _operators, _invited?
 * 1. Los usuarios se unen por decisión propia (comando JOIN)
 * 2. Evitamos duplicar usuarios en múltiples canales idénticos
 * 3. Separamos "configuración del canal" de "estado de usuarios"
 * 4. Los contenedores quedan vacíos automáticamente (constructor por defecto)
 * 
 * Caso de uso típico: Crear un canal con la misma configuración que otro
 * Ejemplo: Channel canalNuevo(canalTemplate); // Misma config, sin usuarios
 * 
 * NOTA: No necesitamos verificar if(this != &other) porque en un constructor
 * el objeto 'this' se está creando y nunca puede ser igual a 'other'.
 */


Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		_name = other._name;
		_topic = other._topic;
		_key = other._key;
		_inviteOnly = other._inviteOnly;
		_topicRestricted = other._topicRestricted;
		_userLimit = other._userLimit;
		_hasPassword = other._hasPassword;
		_hasUserLimit = other._hasUserLimit;		
	}
	return *this;
 }

Channel::~Channel() {}
 
 	// ===========================================
	// ======		MEMBER MANAGEMET 	==========
	// ===========================================


bool	Channel::isMember(Client* member) const {
	if (member == NULL)
		return false;
	return hasClient(member->getNickName());
}
/*	isMember(Client* member):
		→ Solo responde si el cliente ya está en el canal (_members).
		→ No hace ningún cambio.
		→ Se usa para saber si tiene sentido realizar ciertas acciones:
		   - evitar duplicados al hacer JOIN
		   - comprobar si alguien está dentro antes de hacer KICK, etc. */
		   
bool	Channel::addMember(Client* member) {
	if (member == NULL)
		return false;
	if (isMember(member))
		return false;
	_members.push_back(member);
	return true;
}
/*	addMember(Client* member):
		→ Añade el cliente al canal (lo guarda en _members).
		→ Cambia el estado interno del canal.
		→ Solo se ejecuta después de hacer validaciones externas (canJoin, contraseñas...).*/

bool	Channel::leaveChannel(Client* member) {
	if (member == NULL)
		return false;
	if (!isMember(member))
		return false;
	// _members es un std::vector, así que usamos el patrón remove-erase para borrar por valor
	// std::remove reordena y devuelve el rango a borrar, y erase elimina los elementos sobrantes
	_members.erase(std::remove(_members.begin(), _members.end(), member),
	 						_members.end());
	// _operators y _invited son std::set, permiten borrar por valor directamente con erase()
	_operators.erase(member);
	_invited.erase(member);
	return true;
}
/* leaveChannel(Client* member):
		→ Elimina al cliente del canal (de la lista _members)
		→ También lo elimina de _operators y _invited si estaba 
	***_members*** → std::vector:
	  Lista de todos los usuarios que han hecho JOIN al canal. Mejor Usar vector pq..:
	    • Mantiene el orden de llegada (útil para comandos como NAMES)
	    • Se recorre fácilmente para enviar mensajes (broadcast)
	    • Permitimos controlar manualmente los duplicados con isMember()
	***_operators y _invited ****→ std::set:
	  Listas de usuarios con permisos especiales (modo +o y +i). Mejor Usar set pq..:
	    • No permite duplicados (un usuario no puede ser operador dos veces)
	    • El orden no importa
	    • Podemos comprobar o borrar directamente con erase(valor)
*/

bool	Channel::kickUser(Client* kicker, Client* target) {
	if (kicker == NULL || target == NULL)
		return false;
	if (!isOperator(kicker))
		return false;
	if (!isMember(target))			//Validaciones varias del que quiere Kick a otro usuario
		return false;
	if (kicker == target)
		return false;
	leaveChannel(target); 			// doble comprobacion de si esta pero esta bien asi, valido, 
	return true;
}

bool	Channel::inviteUser(Client* inviter, Client* target) {
	if (inviter == NULL || target == NULL)
		return false;
	if (!isOperator(inviter))
		return false;
	if (isMember(target))
		return false;
	if (!isMember(inviter))
		return false;
	if (inviter == target)
		return false;
	_invited.insert(target);
	return true;

}
/* isMember(target) here is preferible→ - Cada Client representa una conexión única, da igual su IP o hostname
 - Es más seguro: compara punteros reales (Client*), evitando ambigüedades q podrían ocurrir si 2 usuarios 
  tienen el mismo nickname temporalmente  o si un nickname cambia entre la validación y el uso.
	_invited (std::set<Client*>): --Lista de usuarios invitados a un canal con modo +i (invite-only).
	Solo los que están en _invited pueden hacer JOIN si el canal está restringido.
	- Se llena con el comando INVITE - Se consulta en canJoin() - Se limpia cuando el usuario entra (ya no necesita invitación)
	Usamos std::set porque: - No permite duplicados - Permite búsquedas y borrado rápidos - No importa el orden */



	// ===========================================
	// ======		OPERATOR MANAGEMET 	==========
	// ===========================================


bool	Channel::addOperator(Client* client) {
	if (client == NULL)					//if (client == NULL) → Seguridad básica
		return false;
	if (!isMember(client))				//f (!isMember(client)) → Solo miembros del canal pueden ser ops (lógica IRC fundamental)
		return false;
	
	_operators.insert(client); 		//std::set(operators) es un contenedor que solo guarda elementos únicos
	return true;	
}
/* addOperator(Client* client):
    → Otorga privilegios de operador (MODE +o) a un cliente del canal.
	 	añade a _operators si pasa validaciones básicas.
		Validaciones que NO hace aquí (van en cmd/mode.cpp):
        ¿Quién ejecuta el comando es operador? → Se valida en MODE command
        ¿Tiene permisos para dar OP? → Se valida en MODE command  
        ¿Ya es operador? → No importa, std::set evita duplicados automáticamente
        ¿Auto-asignación permitida? → Sí se permite en IRC real
        ¿Límite de operadores? → No existe en IRC básico */

bool	Channel::removeOperator(Client* client) {
	if (client == NULL)
		return false;
	if (!isOperator(client))
		return false;
	_operators.erase(client);
	return true;
}
/*Solo comprobamos si es nulo, si tiene privilegios para quitarlos y lo quitamos*/

bool	Channel::isOperator(Client* client) const {
	if (client == NULL)
		return false;
	return _operators.find(client) != _operators.end();
}

bool	Channel::isOperator(const std::string& nickname) const {
	if (nickname.empty())
		return false;
	for (std::set<Client*>::const_iterator opIt = _operators.begin(); opIt != _operators.end(); ++opIt) {
		if ((*opIt)->getNickName() == nickname)
			return true;
	}
	return false;
}
/* This función comprueba si un nickname es operador del canal.El nickname es un string. 
	Aunque _operators guarda punteros a Client, no se puede buscar directamente con find(nickname),
	porque find solo funciona con el mismo tipo que el set (en este caso, Client*), no con std::string.
	Por eso hay que recorrer el bucle especial( recordarlo para set.)
	Se usa const_iterator porque la función es const (in the end of the declaración), y C++ no permite
	usar un iterator normal dentro de una función const.
	Cada elemento del set es un Client*, así que *opIt obtiene el puntero al cliente, y luego con (*opIt)->getNickName()
	se accede al método del objeto apuntado. Los paréntesis son necesarios porque la prioridad de operadores
	en C++ hace que sin ellos el compilador se confunda.
	Si algún nickname coincide con el que se busca, devuelve true. Si no encuentra ninguno, devuelve false al final.
	Usamos la funcion para checkear si alguien tiene permisos de operador antes de ejecutar comandos como KICK, INVITE o cambiar modos.*/


	
	// ===============================================
	// =========	INVITATION MANAGEMET 	==========
	// ===============================================

bool	Channel::addInvited(Client* client) {
	if (client == NULL)
		return false;
	_invited.insert(client);
	return true;
}

bool	Channel::removeInvited(Client* client) {
	if (client == NULL)
		return false;
	if (_invited.find(client) == _invited.end())
		return false;
	_invited.erase(client);
	return true;
}
/*Iteramos  y buscamos el dentro de set invited el cliente , itermos
	y si no encontramos el client y es igual a .end de la lista de invited
	es no encontrado.*/


bool	Channel::isInvited(Client* client) const {
	if (client == NULL)
		return false;
	if (_invited.find(client) == _invited.end())
		return false;
	return true;
}

void	Channel::clearInvited() {
	_invited.clear();
}
/*CLear ya se encarga y funciona bien con contenedores vacios*/



	// ===============================================
	// ========= CHANNEL QUERIES AND VALIDATION ======
	// ===============================================

const	std::vector<Client*> &Channel::whoIsIn() const {
	return _members; 
}

bool	Channel::canJoin(Client* client, const std::string& password) const {
	if (client == NULL)
		return false;
	if (isMember(client))
		return false;
	if (_hasPassword) {
		if (_key != password)
			return false;
	}
	if (_inviteOnly) {
		if (!isInvited(client))
			return false;
	}
	if (_hasUserLimit) {
		if (_members.size() >= _userLimit)
			return false;
	//casteo si hace warnings --size_t con int. (o poner size_t userlimit)
	// o comprobar antes si es el numero negativo para evitar el warning tb.
	}
	return true;
}

bool	Channel::channelIsEmpty() const{
	return _members.empty();
}
// dejar o simplificar , solo sirven por sintaxis funcional
bool	Channel::isEmpty() const {
	return _members.empty();
}

bool	Channel::isFull() const {
	if (!_hasUserLimit)
		return false;
	if (_members.size() >= _userLimit)
		return true;
	return false;
}

int		Channel::getClientCount() const {
	return _members.size();
	
}

int		Channel::getOperatorCount() const {
	return _operators.size();
}


	// ===============================================
	// ============= CLIENT SEARCH METHODS ===========
	// ===============================================


Client* Channel::getClientbyNickname(const std::string& nickname) const {
	for (size_t i = 0; i < _members.size(); i++) {
		if (_members[i]->getNickName() == nickname)
			return _members[i];
	}
	return NULL;	
}

bool	Channel::hasClient(const std::string& nickname) const {
	for (size_t i = 0; i < _members.size(); i++) {
		if (_members[i]->getNickName() == nickname)
			return true;
	}
	return false;
}
/*
 * getClientbyNickname() vs hasClient()
 * Ambas recorren _members buscando un nickname, pero no hacen lo mismo:
 * ------------------------hasClient(nickname)--------------------------
 *     → Devuelve true/false. Solo comprueba si alguien está en el canal.
 *     → Útil para validaciones rápidas:
 *         · NAMES → ¿Está este usuario?
 *         · PRIVMSG → ¿Puede escribir aquí?
 *         · Logs/debug → ¿Quién está conectado
 * ---------------------- getClientbyNickname(nickname)-------------------
 *     → Devuelve un Client* o NULL. Te da el objeto si está.
 *     → Útil cuando necesitas usar ese cliente:
 *         · KICK → para echarlo
 *         · MODE → para cambiarle permisos
 *         · PRIVMSG directo → para enviarle datos
 * -----------Analogía-------------------------------------------------
 *   hasClient()          → "¿Está Juan en casa?"
 *   getClientbyNickname()→ "Pásame a Juan, quiero hablar con él"
 */



	// ===============================================
	// ========== PERMISSION VALIDATION ==============
	// ===============================================
	

bool	Channel::canKick(Client* kicker, Client* target) const {
	if (kicker == NULL || target == NULL)
		return false;
	if (kicker == target)
		return false;
	if (!isOperator(kicker))
		return false;
	if (!isMember(target))
		return false;
	return true;
}

bool	Channel::canSetTopic(Client* client) const {
	if (client == NULL)
		return false;
	if (!isMember(client))
		return false;
	if (_topicRestricted) {
		if (!isOperator(client))
			return false;
	}
	return true;
}



	// ===============================================
	// ============ COMMUNICATION METHODS ============
	// ===============================================


void	Channel::broadcast(const std::string& message, Client* sender) const {
	(void)sender; //de momento
	for (size_t i = 0; i < _members.size(); i++) {
		if (_members[i] == NULL)
			continue ;
		_members[i]->sendMessage(message);
	}
}
// avisa a todos quien entra al canal o se va o topic o kick

void	Channel::broadcastToOthers(const std::string& message, Client* sender) const {
	for (size_t i = 0; i < _members.size(); i++) {
		if (_members[i] == NULL)
			continue ;
		if (_members[i] == sender)
			continue ;
		_members[i]->sendMessage(message);
	}
}
// a todos menos a uno (toOthers)




	// ===============================================
	// ============ IRC PROTOCOL RESPONSES ===========
	// ===============================================


std::string		Channel::getNamesReply() const {
	if (_members.empty())
		return "";
		
	std::string result;
	for (size_t i = 0; i < _members.size(); i++) {
		if (isOperator(_members[i])) {
            result =result + "@";
        }
        result = result + _members[i]->getNickName();
        result = result + " ";
    }
    return result;	
}
/*	getNamesReply():
         Hace la lista de todos los usuarios que están en el canal para el comando NAMES.
         Si es operador le pone @ delante, si no es operador va normal.
    Lets doit:
        - Recorro _members que tiene todos los usuarios del canal
        - Para cada usuario miro si es operador con isOperator()
        - Si es operador le añado @ antes del nickname
        - Siempre le añado el nickname con getNickName()
        - Le pongo un espacio después para separar del siguiente
        - Al final devuelvo todo junto en un string 
    EJ:.    Si en el canal hay alice (operador), bob (normal), charlie (operador)
       ----------------- Devuelve: "@alice bob @charlie " 
    Para el comando NAMES del IRC para ver quien esta conectado */

std::string		Channel::getModes() const {
	
	std::string allmodes = "+";	
	if (_inviteOnly)
		allmodes = allmodes + "i";
	if (_topicRestricted)
		allmodes = allmodes + "t";
	if (_hasPassword)
		allmodes = allmodes + "k";
	if (_hasUserLimit)
		allmodes = allmodes + "l";
	return allmodes;
}
/*  getModes():
    Construimos el string de modos activos del canal
    Empieza con "+" y añade cada modo activo
    Devuelve ejemplos: "+it", "+tkl", "+i" */

std::string		Channel::getTopicReply() const {
	return _topic;
}

std::string		Channel::getChannelInfo() const {
	std::ostringstream channelInfo;
	channelInfo << "Channel " << _name << ": " << _members.size() << " users, "
		<< _operators.size() << " operators, modes " << getModes();
	
	if (!_topic.empty())
		channelInfo << ", topic: " << _topic;
	else
		channelInfo << ", no topic !";
		
	return channelInfo.str();
}
/* mostramos la info aunque este todo vacio... si no añadiriamos if !... .empty() etc..
	se puede mejorar para hacer mas tonterias de mostrar info si no hya nada*/

std::string		Channel::getUserListString() const {
	std::string	showNicknames;
	if (!_members.empty()) {
		for (size_t i = 0; i < _members.size(); i++) { 
			showNicknames = showNicknames + _members[i]->getNickName() + " ";
		}
	}
	else {
		showNicknames = "No Users";
	}
	return showNicknames;
}

	// ===============================================
	// ========== STATIC VALIDATION METHODS ==========
	// ===============================================
	
bool		Channel::isValidChannelName(const std::string& name) {
	if (name.empty())
		return false;
	if (name.length() < 2)
		return false;
	if (name[0] != '#')
		return false;
	if (name.length() > 50)
		return false;
	if (name == "#SERVER" || name == "#NULL" || name == "#ADMIN" 
    || name == "#OPER" || name == "#ROOT")
		return false;
	for (size_t i = 1; i < name.length(); i++) {
		if (name[i] == ' ' || name[i] == ',' || name[i] == '\n'
				|| name[i] == '\r' || name[i] == '\0' || name[i] == '\t'
				|| name[i] == ':' || name[i] == '!' || name[i] == '@'
				|| name[i] == '&' || name[i] == '+')
			return false;
	}
	return true;
	// Minusculas y Mayusculas .. check if lo neceistamos? Se podria implementar
	// devolver cout en mensajes de error  para el usuario ????
}
	
bool		Channel::isValidPassword(const std::string& password) {
	if (password.empty())
		return true;
	 // Contraseña vacía es válida para desactivar el modo +k del canal
    // En IRC: MODE #canal -k (quitar contraseña) internamente usa password=""
	if (password.length() < 3 || password.length() > 14)
		return false;
	for (size_t i = 0; i < password.length(); i++) {
		if (password[i] == ' ' || password[i] == '\n' || password[i] == '\r'
        	|| password[i] == '\0' || password[i] == '\t'
        	|| password[i] == ':' || password[i] == '!')
		return false; 
	}
	return true;
	//devolver mensajes de error para el usuario del worng passwrod?
		/*¿Por qué usamos password.empty()? 
	Porque si está vacía, quitamos la pass del canal, como cuando haces MODE -k.
	Así con setPassword("") se borra bien y _hasPassword se pone en false automáticamente.
	Todo queda sincronizado sin líos.*/
 }
	
bool		Channel::isValidTopic(const std::string& topic) {
	// Topic vacío es válido para quitar el topic del canal
	// En IRC: TOPIC #canal (sin texto) borra el topic
	if (topic.empty())
		return true;
	if (topic.length() > 390)
		return false;
	for (size_t i = 0; i < topic.length(); i++) {
		if (topic[i] == '\0' || topic[i] == '\r' || topic[i] == '\n')
			return false;
	}
	return true;
}


	// ===============================================
	// ========== GETTERS (READ-ONLY ACCESS) =========
	// ===============================================

const std::string& Channel::getChannelName() const {
	return _name;
}

const std::vector<Client*>& Channel::getMembers() const {
	return _members;
}

const std::string& Channel::getPassword() const {
	return _key;
}

const std::string& Channel::getTopic() const{
	return _topic;
}

bool	Channel::isInviteOnly() const {
	return _inviteOnly;
}

bool	Channel::isTopicRestricted() const{
	return _topicRestricted;
}

int		Channel::getUserLimit() const {
	if (_hasUserLimit)
		return _userLimit;
	return 0;
}

bool	Channel::hasPassword() const {
	return _hasPassword;
}

bool	Channel::hasUserLimit() const {
	return _hasUserLimit;
}

	// ===============================================
	// ======= SETTERS (MODIFY CHANNEL STATE) ========
	// ===============================================

void	Channel::setPassword(const std::string &password) {
	_key = password;
	//actualizamos el flag
	_hasPassword = !password.empty();
}
/*
Cambiar contraseña del canal (modo +k):
- Si metes una string (ej: "clave123") ⇒ activa el modo +k y guarda la clave.
- Si metes "" ⇒ desactiva el modo +k y borra la _key.
¿Por que y para que !password.empty()?
- Si la string no esta vacía -> hay contraseña → _hasPassword = true
- Si está vacía → no hay contraseña → _hasPassword = false
 _key y _hasPassword siempre estány deben estar sincronizados.
Solo los Op pueden hacer esto.
*/

void	Channel::setTopic(const std::string &topicName) {
	_topic = topicName;
	
}

void	Channel::setChannelName(const std::string &name) {
	_name = name;	
}

void	Channel::setInviteOnly(bool invite) {
	_inviteOnly = invite;
}
/*
setInviteOnly(bool invite):
Esto activa o desactiva el modo +i del canal. 
Si pones true, solo pueden entrar los que estén invitados. Si pones false, entra quien quiera.
Lo típico de MODE +i o -i llama a esto. Solo los ops pueden usarlo. 
_inviteOnly es el bool que dice si el canal está cerrado, y _invited es la lista de los que tienen pase para entrar.
Ejo: pones setInviteOnly(true), luego haces inviteUser("alice") y ella entra, pero bob no porque no está invitado.
*/

void	Channel::setTopicRestricted(bool restricted) {
	_topicRestricted = restricted;
}

bool	Channel::setUserLimit(int limit) {
	if (limit < 0)
        return false;
    _userLimit = limit;
	// limit = 0 significa "No limit men" (permite infinitos usuarios)
    // limit > 0 significa "limite activo"
    _hasUserLimit = (limit > 0);
    return true;
}




//pablo funcion para una cosa suya

bool Channel::removeClient(Client *client)
{
	if (!client)
		return false;
	
	// Search and delete clients
	std::vector<Client*>::iterator it = std::find(this->_members.begin(), this->_members.end(), client);
	if (it != this->_members.end())
	{
		this->_members.erase(it);

		removeOperator(client);
		removeInvited(client);

		return true;
	}
	
	return false;
}

