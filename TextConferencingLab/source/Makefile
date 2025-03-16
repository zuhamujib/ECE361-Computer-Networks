CC=gcc

all: server deliver
	
server: server.o message.o session.o
	$(CC) -o server server.o message.o session.o
deliver: deliver.o message.o session.o
	$(CC) -o deliver deliver.o message.o session.o -lm

%.o : %.c
	$(CC) -c $< -o $@
clean:
	rm -f *.o server deliver
