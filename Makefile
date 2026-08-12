CC			= cc
CFLAGS	= -Wall -Wextra -Werror -pthread

TARGET	= Codexion
SRCS			=  main.c \
					 parser.c \
					 boss.c \
					 coder.c \
					 context.c \
					 dongle.c \
					 queue.c \
					 simulate.c \
					 codexion.c \
					 cleanup.c

OBJS		=  $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
				$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
				$(CC) $(CFLAGS) -c $< -o $@

clean:
				rm -f $(OBJS)

.PHONY: all clean
