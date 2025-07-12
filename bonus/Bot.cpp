/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 20:59:11 by albelope          #+#    #+#             */
/*   Updated: 2025/07/12 13:25:38 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <sys/socket.h>   // Para crear sockets
#include <netinet/in.h>   // Para configurar la dirección IP
#include <arpa/inet.h>    // Para convertir la IP a binario
#include <unistd.h>       // Para cerrar el socket con close()
#include <cstring>        // Para memset (limpiar estructuras)
#include <cstdlib>        // Para atoi (convertir strings a números)
#include <poll.h>         // Para esperar eventos con poll()
#include <fcntl.h>          //marcar socket O_NONBLOCK
#include "BotCmd.hpp"
#include <errno.h>



Bot::Bot(const std::string& ip, int port, const std::string& pass, const std::string& channel)
    : _ip(ip), _port(port), _pass(pass),
      _nick("Bot42"), _user("Bot42"), _fd(-1)
{
    if (channel[0] != '#')
        _channel = "#" + channel;
    else
        _channel = channel;
}

Bot::~Bot() {
    if (_fd != -1)       // Si el socket its abierto
        close(_fd);      // Lo cerramos correctamente
}



std::string Bot::getChannel() const
{
    return _channel; //Zupergetter dechannel
}


// safeSend es una func que garantiza que se envie todo el mensaje, aunque el socket envie solo parte de los datos en un intento.
// A veces send() puede enviar menos bytes de los que le pedimos, así que aquí nos aseguramos de enviar el mensaje completo repitiendo hasta que todo salga.
bool Bot::safeSend(const std::string& msg)
{
    ssize_t total = 0; // total almacena de cuantos bytes hemos enviado hasta ahora.
    while (total < (ssize_t)msg.size()) // mientras no hayamos enviado todo el mensaje...
    {
        ssize_t n = send(_fd, msg.c_str() + total, msg.size() - total, 0); // intentamos enviar la parte que falta.
        if (n <= 0) 
            return false; // si send() devuelve 0 o un valor negativo, es que ha fallado, asi q salimos con error.
        total += n; // sumamos los bytes enviados para seguir desde el siguiente.
    }
    return true; // cuando hemos enviado todo el mensaje, devolvemos true.
}


void Bot::start() {
    //crear el socet
    _fd = socket(AF_INET, SOCK_STREAM, 0);  // AFINET (es IPv4(127.0.0.1))-- SOCK_STREAM(tipo TCP) -- 0 elige aut el protocolo(TCP)
    if (_fd < 0) {
        std::cerr << "Error: socket gone wrong" << std::endl;
        exit(1);
    }
    std::cout << "Socket created for bot" << std::endl;

    fcntl(_fd, F_SETFL, O_NONBLOCK);
    // Esta linea pone el socket en modo no bloqueante ➜ significa que las operaciones de lectura y escritura no se van a quedar esperando si no hay datos.
    // En los sockets no bloqueantes ➜ si no hay datos, recv() no se queda colgado, simplemente devuelve un error temporal.
    // Esto es obligatorio en el proyecto para poder usar poll() de forma eficiente y que todo el sistema sea reactivo y no se cuelgue.

    // Creamos una estructura que guarda la IP y el puerto del servidor al que me quiero conectar.
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    // sockaddr_in es la estructura que me pide el sistema para poder conectarme.
    // Aquí voy a guardar:
    // - El tipo de red que voy a usar (IPv4 en este caso).
    // - La dir IP del servidor (por ejemplo: 127.0.0.1).
    // - El puerto al que me quiero conecquetar (por ejemplo: 6667).

    serverAddr.sin_family = AF_INET; // tipo de red IPv4
    serverAddr.sin_port = htons(_port); // convierto el puerto al formato que usa la red (big-endian)

    // Convierto la dirección IP que tengo en texto a formato binario que entiende la red.
    if (inet_pton(AF_INET, _ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        std::cerr << "IP Address invalid" << std::endl;
        exit(1);
    }

    // Intento conectar con el servidor usando el socket y la IP que he configurado.
    if (connect(_fd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            std::cerr << "Error: connect() failed" << std::endl;
            exit(1);
        }

        // EINPROGRESS significa que la conexión está en curso ➜ hay que esperar con poll
        struct pollfd pfd;
        pfd.fd = _fd;
        pfd.events = POLLOUT;
        pfd.revents = 0;

        std::cout << "Waiting for connection to complete..." << std::endl;

        if (poll(&pfd, 1, 5000) <= 0) // Timeout de five segundos
        {
            std::cerr << "Error: connection timeout or poll() failed" << std::endl;
            exit(1);
        }

        int error = 0;
        socklen_t len = sizeof(error);
        if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0)
        {
            std::cerr << "Error: getsockopt() after poll indicates error" << std::endl;
            exit(1);
        }
    }
    std::cout << "Bot connected successfully!" << std::endl;

    // Desp de conectar al servidor, ahora mando los comandos IRC obligatorios para iniciar la sesion.
    // Enviar PASS ➜ mando la contraseña que necesita el servidor.
    std::string passCmd = "PASS " + _pass + "\r\n";
    send(_fd, passCmd.c_str(), passCmd.length(), 0);

    // Enviar NICK ➜ mando el nickname que va a tener el bot.
    std::string nickCmd = "NICK " + _nick + "\r\n";
    send(_fd, nickCmd.c_str(), nickCmd.length(), 0);

    // Enviar USER ➜ mando el nombre de usuario que va a usar el bot.
    std::string userCmd = "USER " + _user + " 0 * :" + _user + "\r\n";
    send(_fd, userCmd.c_str(), userCmd.length(), 0);

    // Enviar JOIN ➜ mando el canal al que el bot se va a unir.
    std::string joinCmd = "JOIN " + _channel + "\r\n";
    send(_fd, joinCmd.c_str(), joinCmd.length(), 0);

    // comandos son OBLIGATORIOS en el protocolo IRC para que el bot pueda entrar al servidor y al canal.

    // Preparo la estructura que voy a pasar a poll ➜ poll necesita saber qué sockets tiene que vigilar.
    pollfd pfd;
    pfd.fd = _fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    // Aquí creamos una estructura pollfd ➜ esta estructura le dice a poll() que queremos vigilar nuestro socket (_fd).
    // POLLIN ➜ significa que solo nos interesa saber cuándo hay datos disponibles para leer.

    // Preparo un buffer temporal para leer lo que llegue desde el servidor.
    char buffer[512]; // Este buffer va a almacenar los datos que el servidor me envíe cada vez que poll me avise.
    // El tamaño es 512 ➜ normalmente suficiente para mensajes IRC (aunque pueden llegar fragmentados).

    while (true)
    {
        if (poll(&pfd, 1, -1) <= 0)
            continue;

        if (!(pfd.revents & POLLIN))
            continue;

        ssize_t n = recv(_fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0)
        {
            std::cerr << "server closed\n";
            break;
        }

        buffer[n] = '\0';
        _bufferBot.append(buffer);

        /* Procesar lineas completas */
        size_t pos;
        while ((pos = _bufferBot.find("\r\n")) != std::string::npos)
        {
            std::string line = _bufferBot.substr(0, pos);
            _bufferBot.erase(0, pos + 2);
            std::cout << ">> " << line << '\n';

            if (handlePing(line))
                continue;

            processMessage(line);
        }
    }
}



/* -------------------- detección PING en función aparte -------------- */
bool Bot::handlePing(const std::string& line)
{
    // Esto detecta si el mensaje recibido es un PING.
    // Un PING puede tener dos formatos:
    // Formato 1: "PING :algo" ➜ mensaje directo
    // Formato 2: ":servidor PING :algo" ➜ mensaje precedido por el nombre del servidor

    size_t p = line.find("PING");
    // Buscamos dentro de la línea la palabra "PING".
    // Si no la encontramos ➜ find devuelve npos (no existe).
    // Si no está ➜ no tenemos que responder, porque no es un PING.

    if (p == std::string::npos)
        return false; // No es un PING ➜ el bot no responde ➜ seguimos con el siguiente mensaje.

    // Si hemos encontrado "PING" ➜ extraemos la parte que viene después de los dos puntos (es lo que el servidor espera que devolvamos).
    std::string payload = line.substr(line.find(":"));
    // line.find(":") ➜ busca el primer carácter ":" ➜ en IRC el payload siempre empieza por ":".
    // line.substr(line.find(":")) ➜ extraemos desde ":" hasta el final ➜ ejemplo: ":12345678"

    // Respondemos al servidor con un PONG ➜ este mensaje mantiene viva la conexión.
    safeSend("PONG " + payload + "\r\n");
    // Enviamos el mensaje "PONG :12345678" ➜ es el formato exacto que espera el servidor.
    // Si no respondemos ➜ el servidor piensa que el bot está caído y lo desconectará.

    return true;
    // Devolvemos true ➜ significa que ya hemos procesado este mensaje y no tenemos que hacer nada más con él.
}



void Bot::processMessage(const std::string& line)
{
    // Esto procesa los mensajes PRIVMSG que lleguen al canal donde está el bot.
    // Solo nos interesan los mensajes enviados al canal, no los privados ni de otros sitios.


    //CORREGIRIRC ESTO POSIBLE FALLO
    
    if (line.find("PRIVMSG " + _channel) == std::string::npos)
        return;
    // Buscamos dentro de la línea si aparece "PRIVMSG #canal" ➜ es decir, si el mensaje va dirigido al canal donde está el bot.
    // Si no lo encontramos ➜ no es para nuestro canal ➜ lo ignoramos.

    // Ahora vamos a extraer el texto del mensaje ➜ los mensajes IRC tienen este formato:
    // :nickname PRIVMSG #canal :mensaje
    // El texto empieza después del " :"
    size_t msgPos = line.find(" :");
    if (msgPos == std::string::npos)
        return;
    // Si no encontramos " :" ➜ el formato es inválido ➜ no procesamos el mensaje.

    std::string text = line.substr(msgPos + 2);
    // Extraemos el contenido del mensaje ➜ saltamos el espacio y los dos puntos (por eso +2)
    // Ejemplo:
    // Mensaje completo ➜ ":juan PRIVMSG #canal :Hola mundo"
    // texto ➜ "Hola mundo"

    handleCommands(this, text);

}


