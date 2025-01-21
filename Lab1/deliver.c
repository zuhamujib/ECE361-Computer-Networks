// Task: open a UDP socket and and listen at the specified port
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>


// Some notes: This lab focuses on UDP, which are messgae-oriented i.e. one call from the transmitter corresponds to one call to the reciever
struct in_addr {
    unsigned long s_addr;
};

struct sockaddr_in {
    short sin_family;
    unsigned short sin_port;    // destination port
    struct in_addr sin_addr;    // destination ip address
    char sin_zero[8];
};  // note: in stand for internet as we;re using that protocol suite

int main() {
    
    // To start communication with the client you first create a socket
    // then send the messgae through the socket
    sockaddr_in *socket_addr;
    socket_addr->sin_family = AF_INET;    // internet address family
    // socket_addr->sin_port = 
    // socket_addr->sin_addr = 

    const char *buf = "ftp";
    size_t len = sizeof(buf);

    // ssize_t sendto(socketFd, buf, len, 0, *socket_addr, socket_addrm);

    printf("hello word\n");
}