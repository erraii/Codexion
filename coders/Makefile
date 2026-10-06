NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread
CPPFLAGS	= -D_DEFAULT_SOURCE -Iinclude

SRCS		= src/main.c \
			  src/parse.c \
			  src/time.c \
			  src/init.c \
			  src/sync.c \
			  src/heap.c \
			  src/request.c \
			  src/acquire.c \
			  src/state.c \
			  src/dongle.c \
			  src/routine.c \
			  src/threads.c \
			  src/monitor.c
OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c include/codexion.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re