all: predictors

predictors: main.o
	g++ main.o -o predictors

main.o: main.cpp
	g++ -c main.cpp

clean:
		rm -f *.o predictors
