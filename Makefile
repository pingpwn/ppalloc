CC=gcc


all: main.c
	$(CC) -o main main.c src/ppalloc.c
