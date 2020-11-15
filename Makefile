
CC = g++

CXXFLAGS=-mavx2 -std=c++11 -fopenmp -O3
LDFLAGS=-fopenmp
#

dotp: main.o matoperation.o
	$(CC) $+ -o $@  $(LDFLAGS)

main.o:main.cpp

matoperation.o:matoperation.cpp

clean: 
	rm -f *.o dotp
	
