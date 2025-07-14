/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BotCmd.hpp  	                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 20:42:26 by albelope          #+#    #+#             */
/*   Updated: 2025/07/07 11:01:02 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Bot.hpp"
#include <vector>	// std::vector
#include <unistd.h>	// sleep
#include <ctime>	// time, strftime

void handleCommands(Bot* bot, const std::string& text);

void sendFrames(Bot* bot, std::vector<std::string> frames, int delay);
void botHackCommand(Bot* bot, const std::string& text);
void botTimeCommand(Bot* bot);
void story42(Bot* bot);