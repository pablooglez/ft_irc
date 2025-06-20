# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/29 18:35:50 by pablogon          #+#    #+#              #
#    Updated: 2025/06/20 19:22:23 by pablogon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRC = src/Channel.cpp src/Client.cpp src/RPL.cpp src/Server.cpp \
	src/cmd/quit.cpp src/cmd/pass.cpp src/cmd/nick.cpp src/cmd/join.cpp src/cmd/user.cpp \
	src/main.cpp src/utils.cpp

OBJ = $(SRC:.cpp=.o)

RM = rm -f

CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address

#-------COLORS---------#
BOLD	:= \033[1m
RED     := \033[31;1m
GREEN   := \033[32;1m
YELLOW	:= \033[33m
BLUE	:= \033[34;1m
MAGENTA	:= \033[35m
CYAN	:= \033[36m
WHITE	:= \033[37m
RESET   := \033[0m

all: head $(NAME)

head:
	@echo "$(MAGENTA)$(BOLD)"
	@echo "\t╔════════════════════════════════════════════╗"
	@echo "\t║ ███████╗████████╗     ██╗██████╗  ██████╗  ║"
	@echo "\t║ ██╔════╝╚══██╔══╝     ██║██╔══██╗██╔════╝  ║"
	@echo "\t║ █████╗     ██║        ██║██████╔╝██║       ║"
	@echo "\t║ ██╔══╝     ██║        ██║██╔══██╗██║       ║"
	@echo "\t║ ██║        ██║███████╗██║██║  ██║╚██████╗  ║"
	@echo "\t║ ╚═╝        ╚═╝╚══════╝╚═╝╚═╝  ╚═╝ ╚═════╝  ║"
	@echo "\t╚════════════════════════════════════════════╝"
	@echo "$(RESET)"
	@echo "$(YELLOW)$(BOLD)\t🛠️ 42MLG: $(RESET)$(WHITE)albelope & pablogon$(RESET)"
	@echo "$(PURPLE)\t📂 Project:$(RESET) $(CYAN)ft_irc$(RESET)"
	@echo "$(GREEN)\t🔹 Commands: $(BOLD)all clean fclean re$(RESET)"
	@echo "$(BLUE)\t🧰 Compiler: $(BOLD)$(CC)$(RESET)\n"

$(NAME): $(OBJ)
	@$(CC) $(CPPFLAGS) $(OBJ) -o $(NAME)
	@ echo "$(GREEN)$(NAME) compiled$(RESET)\n"

%.o: %.cpp
	@$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ)

fclean: clean
	@$(RM) $(NAME)
	@ echo "$(RED)$(NAME) deleted$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re