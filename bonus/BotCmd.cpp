/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BotCmd.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 20:17:24 by albelope          #+#    #+#             */
/*   Updated: 2025/07/09 17:12:42 by pablogon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/BotCmd.hpp"
#include <unistd.h> // uso sleep pa las pausas
#include <vector>   // vector pa guardar las frases

// esto es pra conotrlar si han pedido parar la Zuperhistoria
static bool stopRequested = false;

// esta funcion manda cada frase una por una con tiempo entre ellas
void sendFrames(Bot* bot, std::vector<std::string> frames, int delay)
{
    for (size_t i = 0; i < frames.size(); ++i)
    {
        // si han puesto !stop se para la historia y lo aviso
        if (stopRequested)
        {
            bot->safeSend("PRIVMSG " + bot->getChannel() + " :Story stopped.\r\n");
            stopRequested = false; // dejo esto en falso pa la proxima vez
            return;
        }

        // mando la frase al canal
        bot->safeSend("PRIVMSG " + bot->getChannel() + " :" + frames[i] + "\r\n");
        sleep(delay); // espero los segundos que toque antes de la siguiente
    }
}

// esta funcion lanza un whois real al servidor
void botHackCommand(Bot* bot, const std::string& text)
{
    if (text.length() <= 5) { // "!hack" tiene 5 caracteres
        bot->safeSend("PRIVMSG " + bot->getChannel() + " :Usage: !hack [nick]\r\n");
        return;
    }
    
    std::string nick = text.substr(6); // saco el nick despues de "!hack "

    if (nick.empty())
    {
        bot->safeSend("PRIVMSG " + bot->getChannel() + " :Usage: !hack [nick]\r\n");
        return;
    }

    // aviso que voy a hacer el whois
    bot->safeSend("PRIVMSG " + bot->getChannel() + " :[BOT] Hack request for: " + nick + "\r\n");
    // mando el comando real al servidor
    bot->safeSend("WHOIS " + nick + "\r\n");
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

    // mando la historia con 2 segundos entre cada frase
    sendFrames(bot, storyFrames, 2);
}

// aqui detecto que comando han puesto y lanzo la funcion que toca
void handleCommands(Bot* bot, const std::string& text)
{
    if (text.find("!hack") == 0) {
        botHackCommand(bot, text);
        return;
    }
    // si  pones !42story arranco la historia
    if (text == "!42")
    {
        story42(bot);
        return;
    }

    // si han puesto !stop marco que hay que parar
    if (text == "!stop")
    {
        stopRequested = true; // activo la parada
        bot->safeSend("PRIVMSG " + bot->getChannel() + " :Stop request received.\r\n");
        return;
    }
}