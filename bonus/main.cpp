/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 21:01:20 by albelope          #+#    #+#             */
/*   Updated: 2025/07/07 12:19:18 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <iostream>
#include <cstdlib> // para atoi

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./bot <ip> <port> <password> <channel>" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    int port = std::atoi(argv[2]);
    std::string password = argv[3];
    std::string channel = argv[4];

    Bot bot(ip, port, password, channel);
    bot.start();

    return 0;
}