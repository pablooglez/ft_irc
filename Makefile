# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pablogon <pablogon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/29 18:35:50 by pablogon          #+#    #+#              #
#    Updated: 2025/07/15 20:08:09 by pablogon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
BONUS_IRC = ircserv_bonus
BONUS_BOT = bot

# ================ 	Directories  ================= #
SRC_DIR = src
BONUS_DIR = bonus
INCLUDE_DIR = include

# ================ 	Header Files  ================= #
HEADERS = $(INCLUDE_DIR)/Channel.hpp $(INCLUDE_DIR)/Client.hpp $(INCLUDE_DIR)/Server.hpp $(INCLUDE_DIR)/RPL.hpp
BONUS_HEADERS = $(BONUS_DIR)/include/Bot.hpp $(BONUS_DIR)/include/BotCmd.hpp \
				$(BONUS_DIR)/FileTransfer.hpp $(BONUS_DIR)/FileRebuilder.hpp \
				$(BONUS_DIR)/FileManager.hpp $(BONUS_DIR)/Base64.hpp

# ================ 	Source Files  ================= #
SRC = $(SRC_DIR)/Channel.cpp $(SRC_DIR)/Client.cpp $(SRC_DIR)/Server.cpp $(SRC_DIR)/main.cpp $(SRC_DIR)/utils.cpp \
	  $(SRC_DIR)/cmd/quit.cpp $(SRC_DIR)/cmd/pass.cpp $(SRC_DIR)/cmd/nick.cpp $(SRC_DIR)/cmd/join.cpp \
	  $(SRC_DIR)/cmd/names.cpp $(SRC_DIR)/cmd/user.cpp $(SRC_DIR)/cmd/privmsg.cpp $(SRC_DIR)/cmd/topic.cpp \
	  $(SRC_DIR)/cmd/kick.cpp $(SRC_DIR)/cmd/info.cpp $(SRC_DIR)/cmd/list.cpp $(SRC_DIR)/cmd/invite.cpp \
	  $(SRC_DIR)/cmd/part.cpp $(SRC_DIR)/RPL.cpp $(SRC_DIR)/cmd/mode.cpp 

SRC_OBJ = $(SRC:.cpp=.o)
SRC_OBJ_BONUS = $(SRC:.cpp=.bonus.o)

# ================ 	Bonus Source Files  ========== #
BONUS_SRC = $(BONUS_DIR)/FileTransfer.cpp $(BONUS_DIR)/FileRebuilder.cpp \
			$(BONUS_DIR)/FileManager.cpp $(BONUS_DIR)/Base64.cpp

BONUS_OBJ = $(BONUS_SRC:.cpp=.o)

# ================ 	Bot Source Files  ============ #
BOT_SRC = $(BONUS_DIR)/Bot.cpp $(BONUS_DIR)/BotCmd.cpp $(BONUS_DIR)/main.cpp \
		  $(BONUS_DIR)/FileTransfer.cpp $(BONUS_DIR)/Base64.cpp

BOT_OBJ = $(BOT_SRC:.cpp=.o)


# ================ 	Compiler and Flags  ========== #
CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 
RM = rm -f

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

all: header $(NAME)

header:
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
	@echo "$(BLUE)\t🧰 Compiler: $(BOLD)$(CXX)$(RESET)\n"

$(NAME): $(SRC_OBJ)
	@$(CXX) $(CPPFLAGS) $(SRC_OBJ) -o $(NAME)
	@echo "$(GREEN)[✔] Compiled: $(NAME)$(RESET)"

# ================ IRC BONUS executable ============== #
bonus: $(BONUS_IRC) $(BONUS_BOT)

$(BONUS_IRC): $(SRC_OBJ_BONUS) $(BONUS_OBJ)
	@echo "$(CYAN)[Compiling IRC BONUS Executable]$(RESET)"
	@$(CXX) $(CPPFLAGS) -DBONUS -Iinclude -Ibonus $(SRC_OBJ_BONUS) $(BONUS_OBJ) -o $(BONUS_IRC)
	@echo "$(GREEN)[✔] Compiled: $(BONUS_IRC)$(RESET)"

# ============== Bot Executable (independent) ======= #
$(BONUS_BOT): $(BOT_OBJ)
	@echo "$(CYAN)[Compiling BOT Executable]$(RESET)"
	@$(CXX) $(CPPFLAGS) -DBONUS -Iinclude -Ibonus $(BOT_OBJ) -o $(BONUS_BOT)
	@echo "$(GREEN)[✔] Compiled: $(BONUS_BOT)$(RESET)"

# ================ Compilation Rules  ============== #
%.o: %.cpp $(HEADERS)
	@$(CXX) $(CPPFLAGS) -Iinclude -Ibonus -c $< -o $@

%.bonus.o: %.cpp $(HEADERS) $(BONUS_HEADERS)
	@$(CXX) $(CPPFLAGS) -DBONUS -Iinclude -Ibonus -c $< -o $@

# ====================   Cleaning   ================#
clean:
	@$(RM) $(SRC_OBJ) $(SRC_OBJ_BONUS) $(BONUS_OBJ) $(BOT_OBJ)
	@echo "$(YELLOW)Object files deleted$(RESET)"

fclean: clean
	@$(RM) $(NAME) $(BONUS_IRC) $(BONUS_BOT)
	@echo "$(RED)Executables deleted$(RESET)"

re: fclean all

.PHONY: all clean fclean re bonus header