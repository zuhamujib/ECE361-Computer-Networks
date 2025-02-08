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
#include <stdbool.h>
#include "packet.h"

int main(int argc, char *argv[]) {

    // STEP 1: TAKE IN THE LISTEN PORT
    // input should be UDP listen port 
    if (argv[1]==NULL) {
        perror("NO PORT PROVIDED\n");
        exit(1);
    }

    char* listen_port = argv[1];
    int port_value = atoi(listen_port);

    if (port_value < 0) {
        perror("Invalid port number\n");
        exit(1);
    }
    // STEP 2 MAKE THE SOCKET
    int sockfd;
    
    // From Beej's book section 5.3
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("SOCKET ERRORS\n");
        exit(1);
    }

    // STEP 3: BIND THE SOCKET
    struct sockaddr_in s_address;
    memset(&s_address, 0, sizeof(s_address));
    s_address.sin_family 