#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <map>
#include <vector>

class Bot {
    private:
        int             _fd; // fd , bot necesita un socket para conectarse al servidor.
        std::string     _ip;
        int             _port;
        std::string     _pass;
        std::string     _nick;
        std::string     _user;
        std::string     _channel;
        std::string     _bufferBot;

    public:
        Bot(std::string &ip, int port, const std::string &pass, const std::string &channel);
        /*IP ➜ para saber a qué servidor conectar. Puerto ➜ para saber dónde escuchar.
        Password ➜ para autenticar al bot. Canal ➜ para saber a qué canal debe unirse.*/
        ~Bot();


        void start();

};


#endif