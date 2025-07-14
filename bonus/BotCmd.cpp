/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BotCmd.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 20:17:24 by albelope          #+#    #+#             */
/*   Updated: 2025/07/14 17:58:55 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/BotCmd.hpp"

void sendFrames(Bot* bot, std::vector<std::string> frames, int delay)
{
	for (size_t i = 0; i < frames.size(); ++i)
	{
		bot->safeSend("PRIVMSG " + bot->getChannel() + " :" + frames[i] + "\r\n");
		sleep(delay);
	}
}

void botHackCommand(Bot* bot, const std::string& text)
{
	if (text.length() <= 5)
	{
		bot->safeSend("PRIVMSG " + bot->getChannel() + " :Usage: !hack [nick]\r\n");
		return;
	}

	std::string nick = text.substr(6);

	if (nick.empty())
	{
		bot->safeSend("PRIVMSG " + bot->getChannel() + " :Usage: !hack [nick]\r\n");
		return;
	}

	// Crear la secuencia de mensajes del "hack"
	std::vector<std::string> hackMessages;
	hackMessages.push_back("[BOT] 🔍 Scanning target: " + nick);
	hackMessages.push_back("[BOT] 🖥️  Accessing mainframe...");
	hackMessages.push_back("[BOT] 🔐 Bypassing firewall...");
	hackMessages.push_back("[BOT] 📡 Connecting to satellite...");
	hackMessages.push_back("[BOT] 💾 Downloading data...");
	hackMessages.push_back("[BOT] 🔓 Decrypting files...");
	hackMessages.push_back("[BOT] ✅ Hack complete! " + nick + " has been 'hacked' (just kidding! 😄)");

	// Enviar cada mensaje con una pausa entre ellos
	for (size_t i = 0; i < hackMessages.size(); ++i)
	{
		bot->safeSend("PRIVMSG " + bot->getChannel() + " :" + hackMessages[i] + "\r\n");
		if (i < hackMessages.size() - 1)  // No hacer pausa después del último mensaje
			sleep(1);
	}
}

void botTimeCommand(Bot* bot)
{
	// Obtener el tiempo actual (compatible con C++98)
	std::time_t rawTime = std::time(0);
	std::tm* timeInfo = std::localtime(&rawTime);
	
	// Formatear la fecha y hora manualmente
	char timeBuffer[100];
	std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);
	std::string formattedTime(timeBuffer);
	
	// Obtener el día de la semana
	const char* weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	std::string weekday = weekdays[timeInfo->tm_wday];
	
	// Enviar el mensaje con la información del tiempo
	bot->safeSend("PRIVMSG " + bot->getChannel() + " :[BOT] 🕐 Current time: " + formattedTime + " (" + weekday + ")\r\n");
}


void story42(Bot* bot)
{
	std::vector<std::string> storyFrames;

	storyFrames.push_back("In The Hitchhiker's Guide to the Galaxy & MALAGA, 42 is the answer to life, the universe, and everything.");
	storyFrames.push_back("But no one knows what the real question is.");
	storyFrames.push_back("Since 1978, fans have searched for the true meaning of 42.");
	storyFrames.push_back("Philosophers like Plato and Pythagoras believed that numbers hold the deepest knowledge.");
	storyFrames.push_back("In binary, 42 is 101010 — a perfect, balanced pattern.");
	storyFrames.push_back("In ASCII, character 42 is * — the universal wildcard, meaning anything.");
	storyFrames.push_back("Programmers around the world have made 42 their favorite number.");
	storyFrames.push_back("At 42 Malaga, students learn without teachers or classes.");
	storyFrames.push_back("Peer-to-peer learning is the real engine.");
	storyFrames.push_back("The campus teaches you to learn by asking questions, by solving problems, and by building with others.");
	storyFrames.push_back("Learning to learn is the real answer.");
	storyFrames.push_back("The angle that creates rainbows is 42 degrees.");
	storyFrames.push_back("Light crosses a proton in 10^-42 seconds.");
	storyFrames.push_back("There are 42 generations from Abraham to Jesus in the Bible.");
	storyFrames.push_back("42 appears everywhere — maybe by chance, maybe not.");
	storyFrames.push_back("Douglas Adams once said: 'It was just a joke.'");
	storyFrames.push_back("But maybe the joke is on us.");
	storyFrames.push_back("At 42 Malaga, we know the answer.");
	storyFrames.push_back("The answer is 42 or Up2You!");
	storyFrames.push_back("DON'T PANIC. Always carry your towel and eat espetos!.");

	sendFrames(bot, storyFrames, 2);
}

void handleCommands(Bot* bot, const std::string& text)
{
	if (text.find("!hack") == 0)
	{
		botHackCommand(bot, text);
		return;
	}

	if (text == "!42")
	{
		story42(bot);
		return;
	}

	if (text == "!time")
	{
		botTimeCommand(bot);
		return;
	}
}