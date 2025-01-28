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

#define FILENAME_LEN 256

// To start communication with a client:
//      1. Create a socket
//          a. int socket(int domain, int type, int protocol);
//      2. Get the server address info
//      3. Ask user for input from the terminal
//      4. Send the messgage
//      5. Wait for the response from the server
//      6. Handle the server response

// JUST SOME ADDITIONAL NOTES
// remember: argv[0] contains the name of the current program (exe)

// these arguments will be passed in through the terminal on runtime
// deliver <server address> <server port number>

int main(int argc, char* argv[]) {
    // ********************************************************************** Step
    // 1

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

    // ********************************************************************** Step
    // 2 

    struct in_addr server_address;
    inet_pton(AF_INET, argv[1], &server_address);
    int server_port = atoi(argv[2]);

    printf("Number of arguments: %d\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }

    struct sockaddr_in socket_addr;
    memset(&socket_addr, 0, sizeof(socket_addr));  // initialize the mem to 0
    socket_addr.sin_family = AF_INET;              // internet address family
    socket_addr.sin_port = htons(server_port);
    socket_addr.sin_addr = server_address;

    // ********************************************************************** Step
    // 3
    char filename[FILENAME_LEN];
    char ftp[4];

    // Read user input
    while (1) {
        printf("Enter the message to be sent in the format: ftp <filename>\n");

        // Read input and ensure exactly "ftp <filename>" format
        if (scanf("%3s %255s", ftp, filename) == 2) {
            if (strcmp(ftp, "ftp") == 0) {
                break;  // valid input
            }
            else if (strlen(ftp) < 4) {
                printf("Too little charcters.\n");
            } else {
                printf("Input not starting with ftp.\n");
            }
        } else {
            printf("Invalid input format. Expected: ftp <filename>\n");
        }
        while (getchar() != '\n'); // Clear input buffer everytime
        fflush(stdin);
    }

    // checking if the filename entered even exists
    if (access(filename, F_OK) != 0) {
        printf("Invalid filename: file does not exist.\n");
        exit(0);
    }

    // ********************************************************************** Step
    // 4
    printf("Sending message to server.\n");
    ssize_t bytes_sent = sendto(sockfd, ftp, strlen(ftp), 0, (struct sockaddr*)&socket_addr, sizeof(socket_addr));
    if (bytes_sent < 0) {
        printf("Error sending message to server.\n");
        exit(0);
    }

    // ********************************************************************** Step
    // 5
    // waiting for server response
    char message_recieved[FILENAME_LEN];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    printf("Waiting message from server.\n");
    ssize_t bytes_recieved = recvfrom(sockfd, &message_recieved, sizeof(message_recieved), 0, (struct sockaddr*)&from_addr, &from_len);
    if (bytes_recieved < 0) {
        printf("Error recieving message.\n");
        printf("here!\n");
        exit(0);
    }

    // ********************************************************************** Step
    // 6
    // message_recieved[bytes_recieved] = '\0';  // adding a null character at the end to represent a string

    if (strcmp(message_recieved, "yes") == 0) {
        printf("A file transfer can start.\n");
    } else {
        perror("Error recieveing message\n");
        exit(0);
    }

    // Close the socket
    close(sockfd);

    return 0;
}