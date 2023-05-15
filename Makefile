
all:
	g++ -I./3rdparty/getopt-cpp/include -Wall -pedantic -Wno-unused-function -O2 -std=c++11 solve.cpp -o solve
