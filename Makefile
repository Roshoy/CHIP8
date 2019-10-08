LNK = -lSDL2
CC = gcc $(CFLAGS)

.PHONY: all
all: main.c ch8.c ch8.h
	$(CC) main.c -o main.o -c
	$(CC) ch8.c -o ch8.o -c
	$(CC) -o ch8.out main.o ch8.o $(LNK)

.PHONY: run
run: all
	cat $(rom) | ./ch8.out

.PHONY: clean
clean:
	rm -f *.o *.out
