OPTIONS = -Wall -Werror

all: main

main: main.cpp scanner.o parser.o
	g++ main.cpp scanner.o parser.o -o main

scanner.o: scanner.cpp
	g++ -c scanner.cpp

parser.o: parser.cpp
	g++ -c parser.cpp

clean:
	rm -rf *.o main
