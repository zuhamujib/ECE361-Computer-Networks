CC=gcc

all: server deliver
	
server: server.o
	$(CC) -o server server.o
deliver: deliver.o
	$(CC) -o deliver deliver.o -lm

%.o : %.c
	$(CC) -c $< -o $@
clean:
	rm -f *.o server deliver
CC=gcc

all: server deliver
	
server: server.o
	$(CC) -o server server.o
deliver: deliver.o
	$(CC) -o deliver deliver.o -lm

%.o : %.c
	$(CC) -c $< -o $@
clean:
	rm -f *.o server deliver
