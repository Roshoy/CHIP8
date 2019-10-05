CFLAGS = -O1 -Wall
CC = gcc

all: main.c ch8.c ch8.h
    $(CC) main.c -o main.o -c
    $(CC) ch8.c -o ch8.o -c
    $(CC) -o ch8.out main.o ch8.o
