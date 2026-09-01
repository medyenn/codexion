## ============================================================================
## Makefile — Codexion
## ============================================================================

NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread
INCLUDES	= -I src

SRCS		= src/main.c \
			  src/parse_utils.c \
			  src/init.c \
			  src/cleanup.c \
			  src/run.c \
			  src/coder.c \
			  src/coder2.c \
			  src/dongle.c \
			  src/dongle2.c \
			  src/heap.c \
			  src/heap2.c \
			  src/monitor.c \
			  src/utils.c \
			  src/sim_state.c

OBJS		= $(SRCS:.c=.o)

## ---- Rules ----------------------------------------------------------------

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c src/codexion.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
