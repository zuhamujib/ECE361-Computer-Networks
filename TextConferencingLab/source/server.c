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

int total_active = 0;   // TOTAL ACTIVE USERS RIGHT NOW

// STEP 4: CLIENT PACKET HANDLING: start off by dealing with the client message
void handle_client(void *arg) {

    int sock = *((int *)arg);
    free(arg);
    char buffer[BUFFER_SIZE];
    struct message msg;
    int session_id = -1;

    int exited = 0;

    // while we are still retrieveing messages that is until the client does not logout
    while (!exited) {
        memset(buffer, 0, sizeof(buffer));
        memset(&msg, 0, sizeof(msg)); 
        ssize_t recv_size = recv(sock, buffer, sizeof(buffer), 0);
        if (recv_size <= 0) {
            break;
        }

        deserialize_msg(buffer, &msg);  // After we retrieve the client message, deserializing it
        
        printf("Parsed values:\nClient ID: %s\nPassword: %s\n, Size: %d\n", msg.source, msg.data, msg.size);
        fflush(stdout); 

        // Based on the type of packet recieved we want to perform specific actions


        /******************************************************* 1. LOGGING IN A CLIENT  *************************************************/
        if (msg.type == LOGIN) {
            // Step 1: Parse the information from the data
            // Step 2: Check the hardcoded database for the info
            // Step 3: If the user is logged in, save there info to current user databas

            char password[MAX_NAME] = {0};  // extracting message
            sscanf(msg.data, "%s", password);  // Extract password from msg.data

            int logged = 0; // variable keeping track of access to a shared active user database to verify successful login (for mutexes)

            for (int i = 0; hardcoded_users[i][0] != NULL; i++) {   // verifying username and password
            if (strcmp(msg.source, hardcoded_users[i][0]) == 0 && strcmp(password, hardcoded_users[i][1]) == 0) {
                logged = 1;
                break;
                }
            }

            struct message response;    // here we are setting up the response struct
            memset(&response, 0, sizeof(response));
            strcpy(response.source, "server");

            if (logged) {
                // locking database
                pthread_mutex_lock(&clients_mutex);

                // here we parse through adn look for the first empty spot in the database
                // we cant use a counter cuz deleted elements would be black
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    
                    if (clients[i] && strcmp(clients[i]->id, msg.source)) { // need to check if the client is already logged in
                        printf("clients[i]->id = %s, msg.source = %s\n", clients[i]->id, msg.source);
                        // send a LO_NAK
                        response.type = LO_NAK;
                        strcpy(response.data, "Client already logged in");
                        response.size = strlen(response.data);
                        printf("Login failed for user %s\n", msg.source);

                        char buffer[BUFFER_SIZE];   // sending the buffer response (LO_ACK or LO_NAK)
                        serialize_msg(&response, buffer, BUFFER_SIZE);
                        send(sock, buffer, strlen(buffer), 0);
                        continue;

                    } else if (clients[i] == NULL) {   // need to add the new client to the database of active clients

                        // we put in all the client info into the database, and put in an empty string into the session
                        clients[i] = (client_t *)malloc(sizeof(client_t));
                        clients[i]->socket = sock;
                        strcpy(clients[i]->id, msg.source);
                        memset(clients[i]->session, 0, MAX_NAME);  
                        clients[i]->active = 1;
                    }
                }
                
                pthread_mutex_unlock(&clients_mutex);   // technically would unlock the database here

                // may have to lock this value
                total_active++;     // added one to the list of total users 
                // would unlock here

                // setting up the response ack message, tecbnically dont need data but I 
                // put it in caus eidk how the blank will work in the serialize function
                response.type = LO_ACK;
                strcpy(response.data, "Login successful");
                response.size = strlen(response.data);
                printf("User %s logged in successfully\n", msg.source);

            } else {

                // the login failed so we prepare teh fialed login message
                response.type = LO_NAK;
                strcpy(response.data, "Invalid username or password");
                response.size = strlen(response.data);
                printf("Login failed for user %s\n", msg.source);
            }

            char buffer[BUFFER_SIZE];   // sending the buffer response (LO_ACK or LO_NAK)
            serialize_msg(&response, buffer, BUFFER_SIZE);
            send(sock, buffer, strlen(buffer), 0);
        
        /************************************************** 2. EXIT A CLIENT CONNECTION **************************************************/
        } else if (msg.type == EXIT) {
            printf("getting to the exit part\n");
            fflush(stdout);

            // Here we wanna remove the user from the list of active users
            // we would also have to make sure they are removed from any sessions
            // almost all parts might need a mutex i think 

            // Step 1: Go through the sessions and remove the user from any sessions possibly lock seessions info
            // Step 2: Go through active clients list and remove
            int found = 0;

            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] && strcmp(clients[i]->id, msg.source) == 0) {
                    // Once we find the client with the user we are currently working with
                    // we remove the client 

                    printf("Removing client: %s\n", clients[i]->id);
                    
                    // need to close the socket, and free eberyting we used
                    shutdown(clients[i]->socket, SHUT_RDWR);
                    close(clients[i]->socket); 

                    free(clients[i]); 
                    clients[i] = NULL; 

                    total_active = total_active - 1;  // decrement the number of active clients
                    found = 1;
                    break;
                }
            }

            if (found) {
                printf("Client %s successfully removed from active list.\n", msg.source);
            } else { 
                printf("Error: Client %s not found in active list!\n", msg.source);
            }

            pthread_mutex_unlock(&clients_mutex);
            exited = 1;
            shutdown(sock, SHUT_RDWR);
            close(sock);
            break;
        }

        /***************************************************** 3. JOIN A SESSION  *********************************************************/
        else if (msg.type == JOIN) {
            // join
            

        /***************************************************** 4. LEAVE A SESSION  *********************************************************/
        } else if (msg.type == LEAVE_SESS) {
            // leave session
            
        /***************************************************** 5. CREATE A NEW SESSION  ****************************************************/
        } else if (msg.type == NEW_SESS) {
            // new session
            
        /***************************************************** 6. SENDING A MESSAGE  ******************************************************/
        } else if (msg.type == MESSAGE) {
            // message
            
        /***************************************************** 7. QUERYING THE LIST OF ACTIVE CLIENTS  ************************************/
        } else if (msg.type == QUERY) {
            // query
            printf("Received QUERY request from client: %s\n", msg.source);

            pthread_mutex_lock(&clients_mutex);

            char response_data[BUFFER_SIZE] = {0};
            // strcat(response_data, "Active Clients:\n");

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] != NULL) {
                    char temp[MAX_NAME + 20];  // Buffer for each client entry
                    sprintf(temp, "- %s (%s)\n", clients[i]->id, 
                            (strlen(clients[i]->session) > 0) ? clients[i]->session : "No session");
                    strcat(response_data, temp);
                }
            }

            // strcat(response_data, "\nAvailable Sessions:\n");
            pthread_mutex_unlock(&clients_mutex);

            // Prepare and send the response message
            struct message response;
            memset(&response, 0, sizeof(response));
            response.type = QU_ACK;
            response.size = strlen(response_data);
            strcpy(response.source, "server");
            strcpy(response.data, response_data);

            char buffer[BUFFER_SIZE];
            serialize_msg(&response, buffer, BUFFER_SIZE);
            send(sock, buffer, strlen(buffer), 0);

            printf("Sent QUERY response to client %s\n", msg.source);
            
        /***************************************************** 8. SENDING A QUERY ACK  ****************************************************/
        } /* else if (msg.type == QU_ACK) {        // do we neeed this???
            // qu_ack can be removed
        } */
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
