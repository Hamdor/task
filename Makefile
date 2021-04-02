CC=g++
CCFLAGS=-std=c++17 -Wall -Wextra -O3 -pthread
INCLUDE=-Iinclude

example:
	$(CC) $(CCFLAGS) $(INCLUDE) src/main.cpp -o example

benchmark:
	$(CC) $(CCFLAGS) -DSCHED_POLICY=taski::stealing -DCORE_COUNT=4 $(INCLUDE) src/benchmark.cpp -o benchmark_stealing
	$(CC) $(CCFLAGS) -DSCHED_POLICY=taski::sharing -DCORE_COUNT=4 $(INCLUDE) src/benchmark.cpp -o benchmark_sharing

all: clean example benchmark

clean:
	rm -f example
	rm -f benchmark_stealing
	rm -f benchmark_sharing
