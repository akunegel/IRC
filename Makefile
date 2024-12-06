# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: akunegel <akunegel@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/05 11:42:32 by akunegel          #+#    #+#              #
#    Updated: 2024/08/05 17:06:01 by akunegel         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= ircserv

CC		= c++
FLAGS	= -Wall -Wextra -Werror -std=c++98
RM		= rm -rf

SRC		= src/handle_commands.cpp \
			src/handle_operators_commands.cpp \
			src/main.cpp \
			src/mode.cpp \
			src/parsing.cpp \
			src/start.cpp \
			src/utils.cpp
OBJ		= $(SRC:.cpp=.o)

OBJ_DIR = obj
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(OBJ)))

# Colors
GREEN		=	\e[92;5;118m
YELLOW		=	\e[93;5;226m
GRAY		=	\e[33;2;37m
RESET		=	\e[0m
CURSIVE		=	\e[33;3m

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(CURSIVE)$(GRAY) 	- Compiling $(NAME)... $(RESET)\n"
	@$(CC) $(FLAGS) $^ -o $@
	@printf "$(GREEN)    - Executable ready.$(RESET)\n"

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(OBJ_DIR)
	@printf "$(CURSIVE)$(GRAY) 	- Compiling $<... $(RESET)\n"
	@$(CC) $(FLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ_DIR)
	@printf "$(YELLOW)    - Object files removed.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)    - Executable removed.$(RESET)\n"

re: fclean all
