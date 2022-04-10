all: cache-sim

cache-sim: main.o Cache.o
	g++ main.o Cache.o -o cache-sim

main.o: main.cpp
	g++ -c main.cpp

Cache.o: Cache.cpp Cache.h
	g++ -c Cache.cpp

clean:
		rm -f *.o cache-sim