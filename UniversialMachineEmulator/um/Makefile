#
# Makefile for the UM Homework
# 
CC = gcc

IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS  = -l40locality -lcii40 -lm -lbitpack

BIN_DIR = artifacts/bin
EXECS   = $(BIN_DIR)/um

.PHONY: all clean test

all: $(EXECS)

um: $(BIN_DIR)/um

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/um: driver.o instructions.o memory.o bitpack.o | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

writetests: umlabwrite.o umlab.o
	$(CC) $(LDFLAGS) $^ -o $(BIN_DIR)/$@ $(LDLIBS)

test: test.o instructions.o memory.o bitpack.o
# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN_DIR)/um $(BIN_DIR)/writetests test *.o
