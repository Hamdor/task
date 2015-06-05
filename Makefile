CC=clang++
CCFLAGS=-std=c++14 -pthread
INCLUDE=-I include/
all: clean
	${CC} ${CCFLAGS} src/main_stealing.cpp src/work_stealing.cpp ${INCLUDE} -o work_stealing
	${CC} ${CCFLAGS} src/main_sharing.cpp src/work_sharing.cpp ${INCLUDE} -o work_sharing
clean:
	rm -f work_sharing
	rm -f work_stealing
