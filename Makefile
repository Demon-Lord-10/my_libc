
all:
	gcc -o main -Wall -Wshadow -Werror my_libc.c main.c && ./main

clean:
	rm -rf *.o main
