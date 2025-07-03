/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 20:59:11 by albelope          #+#    #+#             */
/*   Updated: 2025/07/03 22:46:28 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bot.hpp"
#include <sys/socket.h>   // Para crear sockets
#include <netinet/in.h>   // Para configurar la dirección IP
#include <arpa/inet.h>    // Para convertir la IP a binario
#include <unistd.h>       // Para cerrar el socket con close()
#include <cstring>        // Para memset (limpiar estructuras)
#include <cstdlib>        // Para atoi (convertir strings a números)
#include <poll.h>         // Para esperar eventos con poll()

Bot::Bot(std::string &ip, int port, const std::string &pass, const std::string &channel) {
    _ip = ip;             // IP donde se conecta el bot
    _port = port;         // Puerto donde escucha el servidor
    _pass = pass;         // Contraseña que necesita el servidor
    _nick = "42bot";      // Nickname que usará el bot 
    _user = "42bot";      // Username que usará el bot 
    _channel = channel;   // Canal al que el bot se conectará
    _fd = -1;             // -1 significa que aún no tenemos socket conectado
    _bufferBot = "";      // Buffer donde acumularemos los mensajes recibidos
}


Bot::~Bot() {
    if (_fd != -1)       // Si el socket está abierto
        close(_fd);      // Lo cerramos correctamente
}


void Bot::start() {
    //crear el socet
    _fd = socket(AF_INET, SOCK_STREAM, 0);  //AFINET (es IPv4(127.0.0.1))-- SOCK_STREAM(tipo TCP) -- 0 elige aut el protocolo(TCP)
    if (_fd < 0) {
        std::cerr << "Error: socket gone wrong" << std::endl;
        exit(1);
    }
    std::cout << "Socket created for bot" << std::endl;


    // Creamos una estructura que guarda la IP y el puerto del servidor al que me quiero conectar.
    struct sockaddr_in serverAddr;
    // sockaddr_in es la estructura que me pide el sistema para poder conectarme.
    // Aquí voy a guardar:
    // - El tipo de red que voy a usar (IPv4 en este caso).
    // - La dirección IP del servidor (por ejemplo: 127.0.0.1).
    // - El puerto al que me quiero conectar (por ejemplo: 6667).
    // Sin esta estructura, el sistema no sabría a qué servidor quiero conectarme.

    // Defino el tipo de conexión que voy a usar.
    serverAddr.sin_family = AF_INET;
    // sin_family es el campo donde le digo qué tipo de red voy a usar.
    // AF_INET significa que estoy usando direcciones IPv4, que son las típicas como 127.0.0.1.
    // Si estuviera usando IPv6, tendría que poner AF_INET6, pero en este proyecto siempre es IPv4.

    // Ahora le digo a qué puerto me quiero conectar.
    serverAddr.sin_port = htons(_port);
    // sin_port es donde guardo el número del puerto al que me quiero conectar (por ejemplo: 6667).
    // La función htons convierte el puerto al formato que usa la red.
    // Los ordenadores pueden guardar los números en diferente orden (endianness), pero la red siempre usa big-endian.
    // htons significa "host to network short" ➜ es decir, convierte el número de mi ordenador al formato que usa la red.
    // Esto es importante porque los ordenadores pueden guardar los números al revés (esto se llama endianness).
    // La red siempre usa big-endian (el byte más importante primero) y si no convierto el puerto, el servidor no lo entendería bien.
    // Ejemplo: si quiero conectarme al puerto 6667 ➜ con htons(6667) lo paso al orden correcto que la red espera.
    // Con htons me aseguro de que el número del puerto esté en el orden correcto para la red.
    // Ejemplo: si quiero conectarme al puerto 6667 ➜ con htons(6667) lo paso al formato que entiende la red.
    /*  h = host       ➜ tu ordenador
        to = to        ➜ convertir a
        n = network    ➜ formato de red
        s = short      ➜ número corto (2 bytes, como los puertos)*/



    // Convierto la dirección IP que tengo en texto a formato binario que entiende la red.
    if (inet_pton(AF_INET, _ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        std::cerr << "IP Adrres invalid" << std::endl;
        exit(1);
    }
    // inet_pton significa "Internet Presentation To Network" ➜ lo que hace es convertir la IP que tengo en texto (por ejemplo: "127.0.0.1")
    // a un formato binario que el socket pueda entender y usar.
    // AF_INET ➜ significa que estamos trabajando con direcciones IPv4.
    // _ip.c_str() ➜ convierte mi string de C++ a una cadena de caracteres tipo C que necesita la función.
    // &serverAddr.sin_addr ➜ es donde se guarda la IP ya convertida en formato de red.
    // Si la IP es inválida o no se puede convertir ➜ la función devuelve <= 0 y el programa se cierra porque no nos podemos conectar.



     // Ahora intento conectar con el servidor usando el socket y la IP que he configurado.
    if (connect(_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error al conectar al servidor" << std::endl;
        exit(1);
    }
    // connect es la función que realmente intenta abrir la conexión con el servidor.
    // Le paso:
    // - El socket que he creado (_fd).
    // - La estructura con la IP y el puerto, convertida a sockaddr porque connect lo pide así.
    // - El tamaño de la estructura que estoy usando.
    // Si la conexión falla, connect devuelve un número negativo y cierro el programa porque no puedo seguir.



    // Después de conectar al servidor, ahora mando los comandos IRC obligatorios para iniciar sesión.
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

    //comandos son OBLIGATORIOS en el protocolo IRC para que el bot pueda entrar al servidor y al canal.
    

    // Preparo la estructura que voy a pasar a poll ➜ poll necesita saber qué sockets tiene que vigilar.
    struct pollfd fds[1];  // Creo un array de un solo pollfd porque solo voy a vigilar mi socket (el que conecta el bot con el servidor).

    fds[0].fd = _fd; // Le digo a poll que quiero vigilar este socket ➜ es el que he creado antes y conecta el bot con el servidor.
    fds[0].events = POLLIN; // POLLIN ➜ significa "avísame cuando haya datos para leer en este socket".
  
    // Preparo un buffer temporal para leer lo que llegue desde el servidor.
    char buffer[512]; // Este buffer va a almacenar los datos que el servidor me envíe cada vez que poll me avise.
    // El tamaño es 512 ➜ normalmente suficiente para mensajes IRC (aunque pueden llegar fragmentados).

    // Ahora entro en un bucle infinito ➜ el bot va a quedarse escuchando todo el rato, sin parar.
    while (true) // Este bucle es la vida del bot ➜ siempre está pendiente de los mensajes del servidor.
    {

        int ret = poll(fds, 1, -1); 
        // Parámetros:
        // fds ➜ es el array donde le he dicho qué sockets vigilar.
        // 1 ➜ solo estoy vigilando un socket.
        // -1 ➜ significa que poll va a esperar para siempre hasta que pase algo (es decir, no tiene tiempo límite).

        // Si poll devuelve > 0 ➜ significa que hay actividad en alguno de los sockets.
        // Y si fds[0].revents & POLLIN ➜ significa que hay datos disponibles para leer en mi socket.
        if (ret > 0 && (fds[0].revents & POLLIN)) 
        {
            // Antes de leer nada, limpio el buffer para evitar basura de lecturas anteriores.
            memset(buffer, 0, sizeof(buffer)); // memset pone a cero todo el buffer.

            // Ahora leo los datos que llegaron del servidor.
            int bytesReceived = recv(_fd, buffer, sizeof(buffer) - 1, 0);
            // recv ➜ recibe datos del socket.
            // _fd ➜ es mi socket del bot.
            // buffer ➜ es donde guardo lo que recibo.
            // sizeof(buffer) - 1 ➜ dejo un espacio para el '\0' final por seguridad (aunque en este caso no es estrictamente necesario).
            // 0 ➜ opciones de lectura normales.

            // Si bytesReceived <= 0 ➜ significa que el servidor cerró la conexión o hubo un error.
            if (bytesReceived <= 0)
            {
                std::cerr << "El servidor ha cerrado la conexión o hubo un error al recibir." << std::endl;
                break; // Salgo del bucle ➜ el bot ya no puede seguir si pierde la conexión.
            }

            // Si he recibido datos ➜ los añado al buffer que guarda los mensajes pendientes de procesar.
            _bufferBot += buffer;
            // _bufferBot es el buffer "acumulador" ➜ a veces los mensajes no llegan completos, así que tengo que ir juntando lo que recibo.

            // Ahora proceso las líneas completas que haya recibido ➜ un mensaje IRC termina con "\r\n".
            size_t pos;
            while ((pos = _bufferBot.find("\r\n")) != std::string::npos)
            {
                // Extraigo la línea completa ➜ desde el principio hasta el final de la línea (antes del \r\n).
                std::string line = _bufferBot.substr(0, pos);

                // Elimino del buffer la línea que ya he procesado ➜ así no la vuelvo a leer.
                _bufferBot.erase(0, pos + 2); // El +2 es para saltarme el \r\n.

                // Muestro por pantalla el mensaje que me ha llegado desde el servidor.
                std::cout << "Mensaje recibido: " << line << std::endl;

                // Si el servidor me envía un PING ➜ tengo que responder con un PONG.
                if (line.find("PING") == 0) // Compruebo si la línea empieza exactamente por "PING".
                {
                    // Para responder ➜ mando "PONG" + el resto de la línea.
                    // Ejemplo: si recibo "PING :123456" ➜ mando "PONG :123456"
                    std::string pingReply = "PONG" + line.substr(4) + "\r\n";

                    // Envío la respuesta al servidor.
                    send(_fd, pingReply.c_str(), pingReply.length(), 0);

                    // Confirmo en consola que he contestado correctamente al PING.
                    std::cout << "Respondido al PING con: " << pingReply;
                }
            }
        }
    }
}