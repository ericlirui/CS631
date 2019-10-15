cc=gcc
CFLAGS=-ansi -g -Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init -Wlogical-op -Wpedantic -Wshadow

bbcp: bbcp.o
	$(cc) -o bbcp  bbcp.o
bbcpm: bbcpm.o
	$(cc) -o bbcpm  bbcpm.o

clean:
	rm bbcp.o
	rm bbcpm.o

