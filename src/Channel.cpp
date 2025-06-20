/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 12:48:09 by albelope          #+#    #+#             */
/*   Updated: 2025/06/20 20:36:58 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Client.hpp"

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
	}
	return true;
}

bool	Channel::channelIsEmpty() const{
	if (!_members.empty())
		return false;
	if (_members.empty())
		return true;
}

bool	Channel::isEmpty() const {
	if (!_members.empty())
		return false;
	if (_members.empty())
		return true;
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