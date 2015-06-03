all: clean
	clang++ -std=c++14 src/main_stealing.cpp src/work_stealing.cpp -g -I include/ -o work_stealing
	clang++ -std=c++14 src/main_sharing.cpp src/work_sharing.cpp -g -I include/ -o work_sharing
clean:
	rm -f work_sharing
	rm -f work_stealing
