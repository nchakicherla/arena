CC = gcc
CFLAGS = -std=c99 -Wall -Wpedantic -Werror -o3 -Wno-unused-function #-luuid
OS := $(shell uname)

mkBinDir := $(shell mkdir -p bin)
mkObjDir := $(shell mkdir -p obj)

BIN = ./bin/example.run

OBJS = ./obj/rkp_arena.o

default: reset $(BIN)
ifeq ($(OS),Darwin) 
	$(BIN)
else 
	valgrind --track-origins=yes --leak-check=full $(BIN)  
endif

reset: clear $(BIN)

run: reset $(BIN)
	$(BIN)

$(BIN): $(OBJS) ./src/example.c ./obj/example.o
	$(CC) $(CFLAGS) ./obj/rkp_arena.o ./obj/example.o -o $(BIN)

./obj/example.o: ./src/example.c ./src/rkp_arena.h ./obj/rkp_arena.o
	$(CC) $(CFLAGS) -c ./src/example.c -o ./obj/example.o

./obj/%.o: ./src/%.c #./src/%.h
	$(CC) $(CFLAGS) -c $< -o $@
	
clear: clear-bin clear-obj

clear-bin:
	-rm ./bin/*

clear-obj:
	-rm ./obj/*
