# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: albelope <albelope@student.42malaga.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/06 20:46:56 by albelope          #+#    #+#              #
#    Updated: 2025/07/08 12:40:59 by albelope         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = bot

SRC = Bot.cpp BotCmd.cpp FileTransfer.cpp  FileRebuilder.cpp FileManager.cpp main.cpp

OBJ = $(SRC:.cpp=.o)

RM = rm -f

CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 


BOLD	:= '\033[1m'
RED     := '\033[31;1m'
GREEN   := '\033[32;1m'
YELLOW	:= '\033[33m'
BLUE	:= '\033[34;1m'
MAGENTA	:= '\033[35m'
CYAN	:= '\033[36m'
WHITE	:= '\033[37m'
RESET   := '\033[0m'

all: head $(NAME)

head:
	@echo "$(MAGENTA)$(BOLD)"
	@echo "\t╔════════════════════════════════════════════╗"
	@echo "\t║ ██████╗  ██████╗ ████████╗                 ║"
	@echo "\t║ ██╔══██╗██╔═══██╗╚══██╔══╝                 ║"
	@echo "\t║ ██████╔╝██║   ██║   ██║                    ║"
	@echo "\t║ ██╔══██═██║   ██║   ██║                    ║"
	@echo "\t║ ██████╔╝╚██████╔╝   ██║                    ║"
	@echo "\t║ ╚═════╝  ╚═════╝    ╚═╝                    ║"
	@echo "\t╚════════════════════════════════════════════╝"
	@echo "$(RESET)"
	@echo "$(YELLOW)$(BOLD)\t🛠️ 42MLG: $(RESET)$(WHITE)albelope & pablogon$(RESET)"
	@echo "$(PURPLE)\t📂 Project:$(RESET) $(CYAN)ft_irc_bot$(RESET)"
	@echo "$(GREEN)\t🔹 Commands: $(BOLD)make clean fclean re$(RESET)"
	@echo "$(BLUE)\t🧰 Compiler: $(BOLD)$(CC)$(RESET)\n"

$(NAME): $(OBJ)
	@$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled successfully!$(RESET)\n"

%.o: %.cpp
	@$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ)
	@echo "$(YELLOW)Object files deleted$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)$(NAME) deleted$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re