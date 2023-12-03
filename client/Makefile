SRCS =	$(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

NAME =  ircserv

CC = c++

FLAGS = #-Wall -Wextra -Werror #-std=c++98

#Colors:
GREEN		=	\e[92;5;118m
GRAY		=	\e[33;2;37m
RESET		=	\e[0m
CURSIVE		=	\e[33;3m

all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(CURSIVE)$(GRAY) 	- Compiling $(NAME)... $(RESET)\n"
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)
	@printf "$(GREEN)    - Executable ready.\n$(RESET)"

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all