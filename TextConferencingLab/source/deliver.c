/* THIS IS THE CLIENT SIDE OF THE CONFERENCE SESSIONS

STEPS TO FOLLOW:
1. Create a socket to establish connections with the server
2. Create a constant stdin while loop to take user input
3. Handle the commands on the stdin file stream
    - LOGIN: go to 4.
4. Connect the socket to the server
5.

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

/* **********************  HELPER FUNCTION PROTOTYES  ****************  */
void handle_Acks(struct message *msg);

/* ******************************************************************** */


int main() {

    /************************************************** STEP 1: CREATE SOCKET ***************************************************/
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);                                                           // SOCK_STREAM for TCP
    if (sockfd < 0) {
        perror("Socket failed");
        return -1;
    }

    /************************************************** STEP 2: TERMINAL STDIN **************************************************/
    char input[INPUT_LEN];
    char client_id[MAX_NAME], password[MAX_NAME], session_id[MAX_NAME];
    char server_ip[MAX_NAME]; 
    int server_port;           
    
    struct sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET;                                                             // internet address family IPv4

    struct message msg;
    memset(&msg, 0, sizeof(msg));                                                                   // initialize memory to 0

    char send_buffer[BUFFER_SIZE]; 
    memset(send_buffer, 0, sizeof(send_buffer));

    char recieve_buffer[BUFFER_SIZE];
    memset(send_buffer, 0, sizeof(send_buffer));

    while (1) {
        memset(&msg, 0, sizeof(msg));                                                              // refresh msg every iteration
        
        fgets(input, sizeof(input), stdin);                                                                 // reading the stdin

        /********************************************** STEP 3: COMMAND HANDLING **************************************************/
        if (strncmp(input, "/login ", 7) == 0) {
            
            sscanf(input, "/login %s %s %s %s", client_id, password, server_ip, server_port);                   // parse the input
            msg.type = LOGIN;

            socket_addr.sin_port = htons(server_port);                                                     // Store server address
            
            if (inet_pton(AF_INET, server_ip, &socket_addr.sin_addr) <= 0) {                        // convert ip address to binary
                perror("Invalid IP address");
                continue;
            }

            if (connect(sockfd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {              // Connect to the server
                perror("Connection failed");
                continue;
            }

            strcpy((char *)msg.source, client_id);                                                             // Store client_id
            strcpy((char *)msg.data, password);                                                                 // Store password
            msg.size = strlen((char *)msg.data);                                                                    // Store size

            serialize_msg(&msg, send_buffer, sizeof(send_buffer));                                          // seriallize the msg

            send(sockfd, &msg, sizeof(msg), 0);                                                     // send the message to server
            
            struct sockaddr_in from_addr;
            socklen_t from_len = sizeof(from_addr);
            ssize_t bytes_recieved = recvfrom(sockfd, &recieve_buffer, sizeof(recieve_buffer), 0, (struct sockaddr*)&from_addr, &from_len);
            if (bytes_recieved < 0) {
                printf("Error recieving message from server.\n");
                exit(1);
            }

            deserialize_msg(recieve_buffer, &msg);                                                       // deseriallize the msg
            handle_Acks(&msg);
            
        } else if (strcmp(input, "/logout") == 0) {
            msg.type = EXIT;
            msg.size = 0;
            
            serialize_msg(&msg, send_buffer, sizeof(send_buffer));                                          // seriallize the msg
            send(sockfd, &msg, sizeof(msg), 0);
            printf("Logged out.\n");

        } else if (strncmp(input, "/joinsession ", 13) == 0) {
            sscanf(input, "/joinsession %s", session_id);
            msg.type = JOIN;

            strcpy((char *)msg.data, session_id);
            msg.size = strlen((char *)msg.data);

            serialize_msg(&msg, send_buffer, sizeof(send_buffer));                                          // seriallize the msg
            send(sockfd, &msg, sizeof(msg), 0);

            struct sockaddr_in from_addr;
            socklen_t from_len = sizeof(from_addr);
            ssize_t bytes_recieved = recvfrom(sockfd, &recieve_buffer, sizeof(recieve_buffer), 0, (struct sockaddr*)&from_addr, &from_len);
            if (bytes_recieved < 0) {
                printf("Error recieving message from server.\n");
                exit(1);
            }

            deserialize_msg(recieve_buffer, &msg);                                                       // deseriallize the msg
            handle_Acks(&msg);

        } else if (strcmp(input, "/leavesession") == 0) {
            msg.type = LEAVE_SESS;

            serialize_msg(&msg, send_buffer, sizeof(send_buffer));                                          // seriallize the msg
            send(sockfd, &msg, sizeof(msg), 0);
            printf("Left session.\n");

        } else if (strncmp(input, "/createsession ", 14) == 0) {
            sscanf(input, "/createsession %s", session_id);
            msg.type = NEW_SESS;
            strcpy((char *)msg.data, session_id);
            msg.size = strlen((char *)msg.data);

            serialize_msg(&msg, send_buffer, sizeof(send_buffer));                                          // seriallize the msg
            send(sockfd, &msg, sizeof(msg), 0);

            struct sockaddr_in from_addr;
            socklen_t from_len = sizeof(from_addr);
            ssize_t bytes_recieved = recvfrom(sockfd, &recieve_buffer, sizeof(recieve_buffer), 0, (struct sockaddr*)&from_addr, &from_len);
            if (bytes_recieved < 0) {
                printf("Error recieving message from server.\n");
                exit(1);
            }

            deserialize_msg(recieve_buffer, &msg);   

        } else if (strcmp(input, "/list") == 0) {
            msg.type = QUERY;

            serialize_msg(&msg, send_buffer, sizeof(send_buffer));                                          // seriallize the msg
            send(sockfd, &msg, sizeof(msg), 0);
            
            struct sockaddr_in from_addr;
            socklen_t from_len = sizeof(from_addr);
            ssize_t bytes_recieved = recvfrom(sockfd, &recieve_buffer, sizeof(recieve_buffer), 0, (struct sockaddr*)&from_addr, &from_len);
            if (bytes_recieved < 0) {
                printf("Error recieving message from server.\n");
                exit(1);
            }

            deserialize_msg(recieve_buffer, &msg);   

        } else if (strcmp(input, "/quit") == 0) {
            msg.type = EXIT;

            serialize_msg(&msg, send_buffer, sizeof(send_buffer));                                          // seriallize the msg
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

// **Function to process acknowledgments from the server**
void handle_Acks(struct message *msg) {
    switch (msg->type) {
        case LO_ACK:
            printf("Successfully logged in %s\n", msg->source);
            break;
        case LO_NAK:
            printf("Login failed: %s\n", msg->data);
            break;
        case JN_ACK:
            printf("Successfully joined session: %s\n", msg->data);
            break;
        case JN_NAK:
            printf("Failed to join session: %s\n", msg->data);
            break;
        case NS_ACK:
            printf("Session created successfully: %s\n", msg->data);
            break;
        case QU_ACK:
            printf("User list & sessions:\n%s\n", msg->data);
            break;
        default:
            printf("Server response: %s\n", msg->data);
            break;
    }
}