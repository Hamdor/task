CC=g++
CCFLAGS=-std=c++17 -Wall -Wextra -pthread
INCLUDE=-Iinclude

CC_SANITIZER_1_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer
CC_SANITIZER_2_FLAGS=-fsanitize=thread

.PHONY: all
.DEFAULT_GOAL := all

all: example test

# Download catch2 header
download:
ifeq (,$(wildcard ./tests/catch2/catch2.hpp))
	mkdir -p tests/catch2 && \
	wget -O tests/catch2/catch2.hpp https://raw.githubusercontent.com/catchorg/Catch2/v2.x/single_include/catch2/catch.hpp
endif

# Build unit tests and compile tests
test: download
	$(CC) $(CCFLAGS) $(CC_SANITIZER_1_FLAGS) $(INCLUDE) tests/compiles_good.cpp -o build/good-address-undefined
	$(CC) $(CCFLAGS) $(CC_SANITIZER_2_FLAGS) $(INCLUDE) tests/compiles_good.cpp -o build/good-thread
	$(CC) $(CCFLAGS) -fprofile-arcs -ftest-coverage $(INCLUDE) -Itests tests/catch_main.cpp tests/index_table_test.cpp tests/atomic_queue_test.cpp \
	tests/xorwow_engine_test.cpp tests/work_item_test.cpp tests/spinlock_test.cpp tests/scheduler_test.cpp -o build/unit-tests
	$(CC) $(CCFLAGS) $(CC_SANITIZER_1_FLAGS) -fprofile-arcs -ftest-coverage $(INCLUDE) -Itests tests/catch_main.cpp tests/index_table_test.cpp tests/atomic_queue_test.cpp \
	tests/xorwow_engine_test.cpp tests/work_item_test.cpp tests/spinlock_test.cpp tests/scheduler_test.cpp -o build/unit-tests-address-undefined
	$(CC) $(CCFLAGS) $(CC_SANITIZER_2_FLAGS) -fprofile-arcs -ftest-coverage $(INCLUDE) -Itests tests/catch_main.cpp tests/index_table_test.cpp tests/atomic_queue_test.cpp \
	tests/xorwow_engine_test.cpp tests/work_item_test.cpp tests/spinlock_test.cpp tests/scheduler_test.cpp -o build/unit-tests-thread


# Build example
example:
	$(CC) $(CCFLAGS) $(INCLUDE) src/main.cpp -o build/example

clean:
	rm -rf build/*
	rm -rf tests/catch2
