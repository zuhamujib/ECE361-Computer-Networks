#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "session.h"
#include "message.h"

#define MAX_CLIENTS 10

// HARD CODED DATABASE
char *hardcoded_users[][2] = {
        {"tya", "test"},
        {"zuha", "vest"},
        {"prof", "best"},
        {NULL, NULL}  // End marker
    };

// ACTIVE USERS DATABASE AND LOCK FOR CLIENTS
client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex;

// TOTAL ACTIVE USERS RIGHT NOW
int total_active = 0;

void handle_client(void *arg) {
    // STEP 4: Here we will actually begin to handle the new client
    // we start off by dealing with the client message

    int sock = *((int *)arg);
    free(arg);
    char buffer[BUFFER_SIZE];
    struct message msg;
    int session_id = -1;

    int exited = 0;


    // while we are still retrieveing messages
    // MIGHT CHANGE THIS TO A BOOL TALKING ABT EXIT
    while (!exited) {

        memset(buffer, 0, sizeof(buffer));
        ssize_t recv_size = recv(sock, buffer, sizeof(buffer), 0);
        if (recv_size <= 0) {
            break;
        }

        deserialize_msg(buffer, &msg);

        // After we retrieve the client message, and deserializing it
        // Based on the type we want to perform a different action
        if (msg.type == LOGIN) {
            // login

            // Step 1: Parse the information from the data
            // Step 2: Check the hardcoded database for the info
            // Step 3: If the user is logged in, save there info to current user database

            // extracting message
            char password[MAX_NAME] = {0};
            sscanf(msg.data, "%s", password);  // Extract password from msg.data

            int logged = 0;

            // actually starting to parse through all the data to compare the source username and the password
            for (int i = 0; hardcoded_users[i][0] != NULL; i++) {
            if (strcmp(msg.source, hardcoded_users[i][0]) == 0 && strcmp(password, hardcoded_users[i][1]) == 0) {
                logged = 1;
                break;
                }
            }

            // here we are setting up the response struct
            struct message response;
            memset(&response, 0, sizeof(response));
            strcpy(response.source, "server");

            if (logged) {

                // locking database
                pthread_mutex_lock(&clients_mutex);

                // here we parse through adn look for the first empty spot in the database
                // we cant use a counter cuz deleted elements would be black
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i] == NULL) {

                        // we put in all the client info into the database, and put in an empty string into the session
                        clients[i] = (client_t *)malloc(sizeof(client_t));
                        clients[i]->socket = sock;
                        strcpy(clients[i]->id, msg.source);
                        memset(clients[i]->session, 0, MAX_NAME);  
                        clients[i]->active = 1;
                    }
                }

                // technically would unlock the database here
                pthread_mutex_unlock(&clients_mutex);

                // added one to the list of total users 
                // may have to lock this value
                total_active++;
                // would unlock here

                // setting up the response ack message, tecbnically dont need data but I 
                // put it in caus eidk how the blank will work in the serialize function
                response.type = LO_ACK;
                strcpy(response.data, "Login successful");
                response.size = strlen(response.data);
                printf("User %s logged in successfully\n", msg.source);
            }
            else {

                // the login failed so we prepare teh fialed login message
                response.type = LO_NAK;
                strcpy(response.data, "Invalid username or password");
                response.size = strlen(response.data);
                printf("Login failed for user %s\n", msg.source);


            }

            // here now we are taking the response message and actually sending the 
            // buffer response
            char buffer[BUFFER_SIZE];
            serialize_msg(&response, buffer, BUFFER_SIZE);
            send(sock, buffer, strlen(buffer), 0);
        }
        else if (msg.type == EXIT) {
            // Here we wanna remove the user from the list of active users
            // we would also have to make sure they are removed from any sessions
            // almost all parts might need a mutex i think 

            // Step 1: Go through the sessions and remove the user from any sessions possibly lock seessions info
            // Step 2: Go through active clients list and remove

            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] && strcmp(clients[i]->id, msg.source) == 0) {
                    // Once we find the client with the user we are currently working with
                    // we remove the client 

                    printf("Removing client: %s\n", clients[i]->id);
                    
                    // need to close the socket, and free eberyting we used
                    close(clients[i]->socket); 
                    free(clients[i]); 
                    clients[i] = NULL; 
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);

            total_active=total_active - 1;
            
        }
        else if (msg.type == JOIN) {
            // join
            
        }
        else if (msg.type == LEAVE_SESS) {
            // leave session
            
       }
        else if (msg.type == NEW_SESS) {
            // new session
            
        }
        else if (msg.type == MESSAGE) {
            // message
            
        }
        else if (msg.type == QUERY) {
            // query
            
        }
        else if (msg.type == QU_ACK) {
            // qu_ack can be removed
                    
        }


    }

    // idt its ever gunan get here but idk if memory leaks will be a porblem
    pthread_mutex_destroy(&clients_mutex);

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
    socklen_t addr_size;
    
    // creating teh mutex for the list of active clients
    pthread_mutex_init(&clients_mutex, NULL);

    while (1) {

        printf("waitin for clients...\n");
        fflush(stdout);
 
        addr_size = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_fd < 0) {
            perror("ACCEPT FAILED");
            continue;
        }

        printf("Connection made starting thread, and sending to handler...\n\n");


        // STEP 3: now we start the threads, and the threads will go to handler function
        // in the handler function based on the input we will handle it

        // basically the hope is to start a thread and it will go to the thread handler 
        pthread_t thread;
        int *new_sock = malloc(sizeof(int));
        *new_sock = client_fd;
        pthread_create(&thread, NULL, (void *)handle_client, (void *)new_sock);

    }



}
    

/* SOME ASSUMPTIONS I MADE WHILE IMPLEMENTING DELIVER.C 
- when a log in is unsuccessful, server sends an error message through a LO_NAK pakcet in the datat field
    - for example: incorrect password
- 

*/
