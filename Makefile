CC=g++
CFLAGS=-Wall

tccTextSearch: main.o auxFunc.o auxFunc.hpp sufTreeNode.hpp sufTree.hpp rootStack.hpp textArrays.hpp vetSearch.hpp
	$(CC) $(CFLAGS) -o tccTextSearch main.o auxFunc.o
	
main.o:
	$(CC) $(CFLAGS) -c main.cpp
	
auxFunc.o:
	$(CC) $(CFLAGS) -c auxFunc.cpp
	
clean:
	rm -rf *.o tccTextSearch
	