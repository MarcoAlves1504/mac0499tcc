CC=g++
CFLAGS=-Wall

tccTextSearch: main.o auxFunc.o auxFunc.hpp skewAlgorithm.o skewAlgorithm.hpp sufTreeNode.hpp sufTree.hpp rootStack.hpp textArrays.hpp vetSearch.hpp
	$(CC) $(CFLAGS) -o tccTextSearch main.o auxFunc.o skewAlgorithm.o
	
main.o:
	$(CC) $(CFLAGS) -c main.cpp
	
auxFunc.o:
	$(CC) $(CFLAGS) -c auxFunc.cpp
	
skewAlgorithm.o:
	$(CC) $(CFLAGS) -c skewAlgorithm.cpp
	
clean:
	rm -rf *.o tccTextSearch
	