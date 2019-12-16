cc=gcc
CFLAGS=-ansi -g -Wall -Werror -Wextra -Wformat=2  -Wshadow -fgnu89-inline

sish: parse.o execute.o sish.o task.o
	$(cc) -o sish -lm parse.o execute.o sish.o task.o

clean:
	rm sish parse.o execute.o sish.o task.o
