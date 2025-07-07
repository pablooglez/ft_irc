/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 12:19:43 by albelope          #+#    #+#             */
/*   Updated: 2025/07/07 12:58:00 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <map>
#include <vector>

class Bot {
    private:
        std::string     _ip;
        int             _port;
        std::string     _pass;
        std::string     _nick;
        std::string     _user;
        std::string     _channel;
        std::string     _bufferBot;
        int             _fd;

    public:
        Bot(const std::string &ip, int port, const std::string &pass, const std::string &channel);
        /*IP ➜ para saber a qué servidor conectar. Puerto ➜ para saber dónde escuchar.
        Password ➜ para autenticar al bot. Canal ➜ para saber a qué canal debe unirse.*/
        ~Bot();


        std::string getChannel() const;

        
        bool safeSend(const std::string& msg);
        bool handlePing(const std::string& line);
        void processMessage(const std::string& line);
        void start();



};


#endif