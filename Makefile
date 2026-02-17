#
# Makefile for the UM Homework
# 
CC = gcc -O2

LDLIBS  = -lcii40-O2 -lbitpack -l40locality -lm
IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS) 
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

EXECS   = um
PROFILE_DIR = profiling/callgrind

.PHONY: all clean test profile-midmark profile-sandmark profile-advent

all: $(EXECS)

um: instructions.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

writetests: umlabwrite.o umlab.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test: test.o

profile-midmark: um
	mkdir -p $(PROFILE_DIR)
	valgrind --tool=callgrind --dump-instr=yes \
		--callgrind-out-file=$(PROFILE_DIR)/midmark.out \
		./um umbin/midmark.um > /dev/null

profile-sandmark: um
	mkdir -p $(PROFILE_DIR)
	valgrind --tool=callgrind --dump-instr=yes \
		--callgrind-out-file=$(PROFILE_DIR)/sandmark.out \
		./um umbin/sandmark.umz > /dev/null

profile-advent: um
	mkdir -p $(PROFILE_DIR)
	valgrind --tool=callgrind --dump-instr=yes \
		--callgrind-out-file=$(PROFILE_DIR)/advent.out \
		./um umbin/advent.umz < advent > /dev/null

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS) writetests *.o
