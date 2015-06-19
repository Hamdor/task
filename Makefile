CC=clang++
CCFLAGS=-std=c++14 -pthread
INCLUDE=-I include/
CONCURRENCY_LEVEL=2

benchmark:
	$(CC) $(CCFLAGS) -DCONCURRENCY_LEVEL=$(CONCURRENCY_LEVEL) -DWORK_STEALING benchmark.cpp src/work_stealing.cpp $(INCLUDE) -o benchmark_stealing
	$(CC) $(CCFLAGS) -DCONCURRENCY_LEVEL=$(CONCURRENCY_LEVEL) -DWORK_SHARING benchmark.cpp src/work_sharing.cpp $(INCLUDE) -o benchmark_sharing
	$(CC) $(CCFLAGS) -DSINGLE_CORE benchmark.cpp -o benchmark_single

all: clean benchmark
	${CC} ${CCFLAGS} -DCONCURRENCY_LEVEL=$(CONCURRENCY_LEVEL) src/main_stealing.cpp src/work_stealing.cpp ${INCLUDE} -o work_stealing
	${CC} ${CCFLAGS} -DCONCURRENCY_LEVEL=$(CONCURRENCY_LEVEL) src/main_sharing.cpp src/work_sharing.cpp ${INCLUDE} -o work_sharing
clean:
	rm -f work_sharing
	rm -f work_stealing
	rm -f benchmark_stealing
	rm -f benchmark_sharing
	rm -f benchmark_single
