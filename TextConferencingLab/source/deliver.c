/* THIS IS THE CLIENT SIDE OF THE CONFERENCE SESSIONS

STEPS TO FOLLOW:
1. Create a socket to establish connections with the server
2. Create a constant stdin while loop to take user input
3. Handle the commands on the stdin file stream


*/

/* **********************  ALL INCLUDE LIBRARIES  ********************** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* **********************  ALL INCLUDE HEADERS  ********************** */
#include "message.h"
#include "deliver.h"

/* **********************  USER DEFINED MACROS  **********************  */
#define INPUT_LEN 100
#define PORT 5000

int main() {

    /************************************************** STEP 1: CREATE SOCKET ***************************************************/
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);                                                           // SOCK_STREAM for TCP

    /************************************************** STEP 2: TERMINAL STDIN **************************************************/
    char input[INPUT_LEN];
    char client_id[MAX_NAME], password[MAX_NAME], session_id[MAX_NAME];
    char server_ip[MAX_NAME]; 
    int server_port;           
    
    struct sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET;                                                                   // internet address family

    while (1) {
        fgets(input, sizeof(input), stdin);                                                                 // reading the stdin

        struct message msg;
        memset(&msg, 0, sizeof(msg));                                                                   // initialize memory to 0

        /********************************************** STEP 3: COMMAND HANDLING **************************************************/
        if (strncmp(input, "/login ", 7) == 0) {
            sscanf(input, "/login %s %s", client_id, password, server_ip, server_port);
            msg.type = LOGIN;

            socket_addr.sin_port = server_port;
            // socket_addr.sin_addr = server_ip;
        
            // Set up server address
            socket_addr.sin_port = htons(server_port);
            if (inet_pton(AF_INET, server_ip, &socket_addr.sin_addr) <= 0) {
                perror("Invalid IP address");
                continue;
            }

            // Connect to the server
            if (connect(sockfd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
                perror("Connection failed");
                continue;
            }

            strcpy((char *)msg.source, client_id);
            strcpy((char *)msg.data, password);
            msg.size = strlen((char *)msg.data);
            send(sockfd, &msg, sizeof(msg), 0);
            printf("Logging in as %s...\n", client_id);

        } else if (strcmp(input, "/logout") == 0) {
            msg.type = EXIT;
            send(sockfd, &msg, sizeof(msg), 0);
            printf("Logged out.\n");

        } else if (strncmp(input, "/joinsession ", 13) == 0) {
            sscanf(input, "/joinsession %s", session_id);
            msg.type = JOIN;
            strcpy((char *)msg.data, session_id);
            msg.size = strlen((char *)msg.data);
            send(sockfd, &msg, sizeof(msg), 0);
            printf("Joining session: %s\n", session_id);

        } else if (strcmp(input, "/leavesession") == 0) {
            msg.type = LEAVE_SESS;
            send(sockfd, &msg, sizeof(msg), 0);
            printf("Left session.\n");

        } else if (strncmp(input, "/createsession ", 14) == 0) {
            sscanf(input, "/createsession %s", session_id);
            msg.type = NEW_SESS;
            strcpy((char *)msg.data, session_id);
            msg.size = strlen((char *)msg.data);
            send(sockfd, &msg, sizeof(msg), 0);
            printf("Created session: %s\n", session_id);

        } else if (strcmp(input, "/list") == 0) {
            msg.type = QUERY;
            send(sockfd, &msg, sizeof(msg), 0);
            printf("Requesting user/session list...\n");

        } else if (strcmp(input, "/quit") == 0) {
            msg.type = EXIT;
            send(sockfd, &msg, sizeof(msg), 0);
            printf("Exiting client.\n");
            break;

        } else {
            /* Assume it's a normal message */
            msg.type = MESSAGE;
            strcpy((char *)msg.data, input);
            msg.size = strlen((char *)msg.data);
            send(sockfd, &msg, sizeof(msg), 0);
        }
    }
    return 0;
}
