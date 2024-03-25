CC=g++
CFLAGS=-Wall -Wextra -Werror -O2
DFLAGS=-fsanitize=address,undefined -g
WORK_FILE=./main.cpp ./pool.cpp

all: build

build:
	$(CC) $(CFLAGS) $(WORK_FILE) -o main

debug:
	$(CC) $(CFLAGS) $(DFLAGS) $(WORK_FILE) -o main_d
	./main_d

clean:
	rm -rf main main_d

rebuild: clean all