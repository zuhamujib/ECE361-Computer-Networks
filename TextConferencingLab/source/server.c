#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "session.h"
#include "message.h"

// functions to help serialize and deserialize with the delimter ":"
void serialize_msg(struct message *msg, char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "%u:%u:%s:%s", msg->type, msg->size, msg->source, msg->data);
    // puts msh into proper format
}

void deserialize_msg(const char *buffer, struct message *msg) {
     sscanf(buffer, "%d:%d:%49[^:]:%1023[^\n]", &(msg->type), &(msg->size), msg->source, msg->data);
     // retrieves the value, the size, the name up to 49 bytes, and the data up to 1023 bytes
}


int main(int argc, char *argv[]) {
    // STEP 1: Retriving the port number from user and checking the port number
    if (argv[1]==NULL) {
        perror("NO PORT PROVIDED\n");
        exit(1);
    }

    // setting up initial socket connection stuff
    char* listen_port = argv[1];
    int port = atoi(listen_port);

    // trying to create a socket
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("SOCKET FAILURE \n");
        exit(1);
    }

    // setting up the socket info stuff
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // binding our IP to the port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("BIND FAILURE");
        exit(1);
    }

    // starting to listen and trying to see if we can listen on our TCP connection
    if (listen(server_fd, MAX_CLIENTS_PER_SESSION) < 0) {
        perror("LISTEN FAILUTRE");
        exit(1);
    }

    printf("server on port %d...\n", port);

    // STEP 2: BEGING MAIN LOOP TO RETIRVE INFO AND MAKE CONNECRIONS
    // the while loop will start here 
    // Will need to handle multiple threads and connections starting from this point and addressing teh user messages

    // setting up client stuff to accept messages
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1) {

        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("ACCEPT FFAILED");
            continue;
        }

    }



}
    

