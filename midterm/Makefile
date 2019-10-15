cc=gcc
CFLAGS=-ansi -g -Wall -Werror -Wextra -Wformat=2  -Wshadow -Wlogical-op -fgnu89-inline

ls: ls.o cmp.o dlist.o print.o
	$(cc) -o ls -lm ls.o cmp.o dlist.o print.o

clean:
	rm ls.o cmp.o dlist.o print.o
