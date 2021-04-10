CC=g++
CCFLAGS=-std=c++17 -Wall -Wextra -pthread
INCLUDE=-Iinclude

CC_SANITIZER_1_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer
CC_SANITIZER_2_FLAGS=-fsanitize=thread

.PHONY: all
.DEFAULT_GOAL := all

all: clean example test

test:
	$(CC) $(CCFLAGS) $(CC_SANITIZER_1_FLAGS) $(INCLUDE) tests/compiles_good.cpp -o build/good-address-undefined
	$(CC) $(CCFLAGS) $(CC_SANITIZER_2_FLAGS) $(INCLUDE) tests/compiles_good.cpp -o build/good-thread

example:
	$(CC) $(CCFLAGS) $(INCLUDE) -O3 src/main.cpp -o build/example

clean:
	rm -rf build/*
