CX = g++

CXXFLAGS = -g -Wall 

all: key-store

key-store: key-store.o
	$(CXX) $(CXXFLAGS) -o key-store key-store.o

key-store.o:

clean:
	rm -f key-store *.o *.gch
