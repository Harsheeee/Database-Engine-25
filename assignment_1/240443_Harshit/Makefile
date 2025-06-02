CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: main

main: main.o btree.o
	$(CC) $(CFLAGS) -o main main.o btree.o

main.o: main.c btree.h
btree.o: btree.c btree.h

clean:
	rm -f *.o main