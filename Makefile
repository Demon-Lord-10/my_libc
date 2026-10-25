
all:
	gcc -o main -Wall -Wshadow -Werror my_libc.c main.c

clean:
	rm -rf *.o main
