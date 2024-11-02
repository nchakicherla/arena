CC = gcc
CFLAGS = -std=c99 -Wall -Wpedantic -Werror -o3 #-luuid
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)

BIN = ./bin/main.run

MAIN = 	./obj/main.o

default: reset $(BIN)
ifeq ($(OS),Darwin) 
	$(BIN)
else 
	valgrind --track-origins=yes --leak-check=full $(BIN)  
endif

reset: clear $(BIN)

run: reset $(BIN)
	$(BIN)

$(BIN): $(MAIN)
	$(CC) $(CFLAGS) $(MAIN) -o $(BIN)

./obj/main.o: ./src/main.c
	$(CC) $(CFLAGS) -c ./src/main.c -o ./obj/main.o
	
clear: clear-bin

clear-bin:
	-rm ./bin/*
