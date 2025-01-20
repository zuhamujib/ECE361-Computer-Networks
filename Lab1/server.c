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

int main(int argc, char *argv[]) {

    // STEP 1: TAKE IN THE LISTEN PORT
    // input should be UDP listen port 
    listen_port = argv[1];
    // LATER: make sure to check if this is valid number

    // STEP 2: CREATE THE SOCKET
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("errorsocketdidntowrk:");
    }

    // STEP 3: BIND THE SOCKET TO A PORT
    

    


    printf("hello word\n");



    return 0;
}

    // from Beej's book seciton 5 on get addr
    // int status;
    // struct addrinfo hints;
    // struct addrinfo *servinfo; // will point to the results

    // memset(&hints, 0, sizeof hints); // make sure the struct is empty
    // hints.ai_family = AF_INET; // don't care IPv4 or IPv6
    // hints.ai_socktype = SOCK_DGRAM; // TCP stream sockets
    // hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    // if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
    // fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    // exit(1);
    // }
    ////////////////////////////////////////////
    // must free 
    // freeaddrinfo(servinfo);