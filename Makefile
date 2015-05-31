all: clean
	clang++ -std=c++14 main.cpp work_sharing.cpp -o work_sharing

clean:
	rm -f work_sharing
