OPTIONS = -Wall -Werror

all: main

main: main.cpp scanner.o parser.o executer.o
	g++ main.cpp scanner.o parser.o executer.o -o main

scanner.o: scanner.cpp
	g++ -c scanner.cpp

parser.o: parser.cpp
	g++ -c parser.cpp
	
executer.o: executer.cpp
	g++ -c executer.cpp

clean:
	rm -rf *.o main
