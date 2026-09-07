NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread
INCLUDES	= -I include

SRCS		= src/main.c \
			  src/parse.c \
			  src/time.c \
			  src/init.c \
			  src/sync.c \
			  src/heap.c \
			  src/request.c \
			  src/acquire.c \
			  src/state.c \
			  src/wait.c \
			  src/release.c \
			  src/log.c \
			  src/sleep.c \
			  src/routine.c \
			  src/threads.c \
			  src/monitor.c
OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c include/codexion.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re