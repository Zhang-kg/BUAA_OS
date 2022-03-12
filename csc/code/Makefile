CC=gcc
CFLAGS=-I../include

all: main.o fibo.o
	$(CC) main.o fibo.o -o ../fibo $(CFLAGS)

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

fibobk.o: fibo.c
	$(CC) -c fibo.c -o fibo.o $(CFLAGS)

.PHONY: clean
clean: 
	rm -rf *.o	
