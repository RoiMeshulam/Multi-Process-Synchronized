CC=g++
AR=ar
FLAGS= -Wall -g

all: client server
test: test2

client:  Client.o
	$(CC) $(FLAGS) -o Client Client.o

server:  Server.o
	$(CC) $(FLAGS)  -o Server Server.o

Server.o: Server.cpp
	$(CC) $(FLAGS)  -c Server.cpp

Client.o: Client.cpp
	$(CC) $(FLAGS) -c Client.cpp

test2:  Test.o
	$(CC) $(FLAGS) -o Test Test.o

Test.o: Test.c
	$(CC) $(FLAGS) -c Test.c

.PHONY: clean all
clean:
	rm -f *.o Client Server Test