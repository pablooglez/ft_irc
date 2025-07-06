/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botcommand.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 20:42:26 by albelope          #+#    #+#             */
/*   Updated: 2025/07/06 20:57:12 by albelope         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOTCOMMAND_HPP
#define BOTCOMMAND_HPP

#include "bot.hpp"
#include <vector>


void handleCommands(Bot* bot, const std::string& text);


void sendFrames(Bot* bot, std::vector<std::string> frames, int delay);
void botHackCommand(Bot* bot, const std::string& text);
void story42(Bot* bot);

#endif
