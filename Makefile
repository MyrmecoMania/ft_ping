NAME = ft_ping

FLAG = -Wall -Wextra -Werror -g3

SRC = main.c src/init.c src/utils.c src/errors.c

OBJ = $(SRC:.c=.o)
.c.o:
		gcc $(FLAG) -c $< -o $(<:.c=.o)

$(NAME): $(OBJ)
		gcc $(OBJ) -o $(NAME)
all: $(NAME)

clean:
		rm -f $(OBJ) 

fclean: clean
		rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
