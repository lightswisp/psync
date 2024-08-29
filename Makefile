INCLUDE_DIR=include 
OBJ_DIR=build
SRC_DIR=src
OUT=server.out

DEBUG_FLAGS=-ggdb
WARNING_FLAGS=-Wall -Werror -pedantic
LIBS=-lm
CFLAGS=-I$(INCLUDE_DIR) $(WARNING_FLAGS) $(DEBUG_FLAGS) $(LIBS)
CC=gcc

SRCS := $(shell find $(SRC_DIR) -name "*.c")
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: rendezvous-server

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

rendezvous-server: $(OBJS)
	$(CC) $^ -o $(OUT)

clean:
	@rm build/*

PHONY: all clean
