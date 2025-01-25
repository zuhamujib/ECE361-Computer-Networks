CC=gcc

all: server deliver
	
server: server.o
	$(CC) -o server server.o
deliver: deliver.o
	$(CC) -o deliver deliver.o

%.o : %.c
	$(CC) -c $< -o $@
clean:
	rm -f *.o server deliver
