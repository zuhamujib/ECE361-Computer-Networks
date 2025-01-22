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

    if (argv[1]==NULL) {
        printf("NO PORT PROVIDED\n");
        exit(1);
    }


    char* listen_port = argv[1];
    int port_value = atoi(listen_port);
    // LATER: make sure to check if this is valid number

    // STEP 2: GET ADDRESS INFO
    // from Beej's book seciton 5.1 
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo; // will point to the results

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_INET; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me
    //hints.ai_addr = htons(port_value); // filling in with the port value

    if ((status = getaddrinfo(NULL, listen_port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
    }

    // STEP 3: MAKE THE SOCKET
    int sockfd;
    
    // From Beej's book section 5.3
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, 0)) < 0) {
        printf("errorsocketdidntowrk:");
        // possibly fix this error more later
    }

    // STEP 4: BIND THE SOCKET
    // From Beej's book from section 5.3
    bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    //does this need to be error checked?

    // STEP 5: MUST FREE
    freeaddrinfo(servinfo);


// LISTEN BEFORE OR AFTERING FREEING? PROBABLY AFTER


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



//     struct addrinfo hints, *res;
//     int sockfd;
//     // first, load up address structs with getaddrinfo():

//     memset(&hints, 0, sizeof hints);
//     hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_flags = AI_PASSIVE; // fill in my IP for me
 
//     getaddrinfo(NULL, "3490", &hints, &res);

//     // make a socket:

//     sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// /   / bind it to the port we passed in to getaddrinfo():
//     bind(sockfd, res->ai_addr, res->ai_addrlen);