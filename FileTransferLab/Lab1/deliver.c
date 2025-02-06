// Task: open a UDP socket and and send message at the specified port
// Some notes: This lab focuses on UDP, which are messgae-oriented i.e. one call
// from the transmitter corresponds to one call to the reciever
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "deliver.h"

#define FILE_LEN 256
#define INPUT_LEN 128

// Lab 1
/*
To start communication with a client:
     1. Create a socket
         a. int socket(int domain, int type, int protocol);
     2. Get the server address info
     3. Ask user for input from the terminal
     4. Send the messgage
     5. Wait for the response from the server
     6. Handle the server response
        a. Success! -> 7.
        b. Failure:( -> 10.
     7. Find the file size
     8. Segment into packets of size 1000 bytes
     9. Send packet stop-and-wait for acknowledgement
     10. Close the socket
*/


// Lab 2
/*
Please remember that while the beginning of the packet is in fact just plain text, the data
portion of the packet may in fact contain binary data. This means that you should not use
string manipulation functions available in C for the data field or for the whole packet. Only
the first part of the packet before data is really a string.
*/

// JUST SOME ADDITIONAL NOTES
// remember: argv[0] contains the name of the current program (exe)

// these arguments will be passed in through the terminal on runtime
// deliver <server address> <server port number>

int main(int argc, char* argv[]) {
    // ********************************************************************** Step 1

    if (argv[1] == NULL || argv[2] == NULL) {
        perror("INVALID EXECUTION COMMAND\n");
        exit(1);
    }
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);       // we used SOCK_DGRAM for UDP

    // From Beej's section 5.3
    if (sockfd < 0) {
        perror("error creating socket\n");
        // possibly fix this error more later
    }

    // ********************************************************************** Step 2 
    struct in_addr server_address;
    inet_pton(AF_INET, argv[1], &server_address);
    int server_port = atoi(argv[2]);

    if (server_port < 0) {
        perror("Invalid port number\n");
        exit(EXIT_FAILURE);
    }
    printf("Number of arguments: %d\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }

    struct sockaddr_in socket_addr;
    memset(&socket_addr, 0, sizeof(socket_addr));  // initialize the mem to 0
    socket_addr.sin_family = AF_INET;              // internet address family
    socket_addr.sin_port = htons(server_port);
    socket_addr.sin_addr = server_address;

    // ********************************************************************** Step 3
    char filename[INPUT_LEN];
    char ftp[INPUT_LEN];
    char input[FILE_LEN];   // reading the full user input

    // Read user input constantly until a correct input is input format is given
    while (1) {
        printf("Enter the message to be sent in the format: ftp <filename>\n");

        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0'; // m,aking sure to remove any enldine characters instead ending it with a eof character for string

            // Parse the input and check for exactly two inputs
            if (sscanf(input, "%255s %255s", ftp, filename) == 2 && strcmp(ftp, "ftp") == 0) {
                // printf("Input 1: %s\n", ftp);
                // printf("Input 2: %s\n", filename);
                break;
            } else {
                printf("Incorrect format of input.\n");
            }
        } else {
            printf("Failed to read input.\n");
        }
        fflush(stdin);  // restting the user input so no errors happen
    }

    // checking if the filename entered even exists
    if (access(filename, F_OK) != 0) {
        printf("filename does not exist.\n");
        exit(1);
    }

    // ********************************************************************** Step 4
    printf("Sending message to server.\n");
    ssize_t bytes_sent = sendto(sockfd, ftp, strlen(ftp), 0, (struct sockaddr*)&socket_addr, sizeof(socket_addr));
    if (bytes_sent < 0) {
        printf("Error sending message to server.\n");
        exit(1);
    }

    // ********************************************************************** Step 5
    // waiting for server response
    char message_recieved[FILE_LEN];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    printf("Waiting message from server.\n");
    ssize_t bytes_recieved = recvfrom(sockfd, &message_recieved, sizeof(message_recieved), 0, (struct sockaddr*)&from_addr, &from_len);
    if (bytes_recieved < 0) {
        printf("Error recieving message from server.\n");
        exit(1);
    }

    // ********************************************************************** Step 6
    message_recieved[bytes_recieved] = '\0';  // adding a null character at the end to represent a string

    if (strcmp(message_recieved, "yes") == 0) {
        printf("A file transfer can start.\n");
    } else {
        perror("Error recieveing message\n");
        exit(1);
    }

    // ********************************************************************** Step 7
    FILE *file = fopen(filename);
    if (!file) {
        printf("Error reading file.\n");
        return -1;
    }

    char buffer[NUM_FRAGMENTS] = {0};    // char is 1 byte
    int num_packets = 1;
    size_t size;
    while (!file) {
        size = fread(buffer, 1, NUM_FRAGMENTS, file); // reading 1000 bytes

        // build a packet
        struct packet pack;
        pack.total_frag = 1;
        pack.frag_no = num_packets;
        pack.size = size;
        strcpy(pack.filename, filename);
        

        // send the packet to server

        // wait for acknowledgement
        bytes_recieved = recvfrom(sockfd, &message_recieved, sizeof(message_recieved), 0, (struct sockaddr*)&from_addr, &from_len);
        if (bytes_recieved < 0) {
            printf("Error recieving message from server.\n");
            exit(1);
        }
        message_recieved[bytes_recieved] = '\0';  // adding a null character at the end to represent a string

        if (strcmp(message_recieved, "ACK") == 0) {
            printf("packet %d acknowledged\n", num_packets);
            num_packets++;
        } else if (strcmp(message_recieved, "NOT_ACK") == 0) {
            printf("packet not acknowledged, sending again!\n");
            buffer -= NUM_FRAGMENTS;
        }
    }

    if (feof(file)) {
        printf("End of file reached.\n");
    } else if (ferror(file)) {
        perror("Error reading file.\n")
    }

    fclose(file);

    // Close the socket
    close(sockfd);

    return 0;
}