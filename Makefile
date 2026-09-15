CC			= cc
CFLAGS	= -g -Wall -Wextra -Werror -pthread

NAME	= codexion
SRCS			=  main.c \
					 parser.c \
					 coder.c \
					 context.c \
					 dongle.c \
					 simulate.c \
					 codexion.c \
					 monitor.c \
					 heap.c \
					 heap_algo.c \
					 coder_action.c \
					 time.c \
					 coder_time.c \
					 utils.c \
					 dongles_utils.c \
					 cleanup.c

OBJS		=  $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
				$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
				$(CC) $(CFLAGS) -c $< -o $@

clean:
				rm -f $(OBJS)

fclean: clean
				rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
