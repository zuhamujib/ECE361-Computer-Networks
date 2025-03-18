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

//  THE LOCK FOR THE SESSIONS (THE ACC ARRAY IS IN THE HEADER AS EXTERN)
pthread_mutex_t sessions_mutex;  

// function to find a client based on the sock
//this is for like after your login because we dont know the client 

// STEP 4: CLIENT PACKET HANDLING: start off by dealing with the client message
void handle_client(void *arg) {

    int sock = *((int *)arg);
    free(arg);
    char buffer[BUFFER_SIZE];
    struct message msg;
    int session_id = -1;

    int exited = 0;

    // the special client will be set after the login
    client_t *special_client = NULL;  
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

                        // setting the special client
                        special_client = clients[i];
                        break;
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
            // STEP 1: lock and find the session index we wanna join
            // STEP 2: create msg
            // STEP 3: handle all cases: no session index (not in session), 

            pthread_mutex_lock(&sessions_mutex);

            int session_index = -1;
            for (int i = 0; i < MAX_SESSIONS; i++) {
                if (sessions[i].client_count > 0 && strcmp(sessions[i].session_id, msg.data) == 0) {
                    session_index = i;
                    break;
                }
            }

            struct message response;
            memset(&response, 0, sizeof(response));
            strcpy(response.source, "server");


            if (session_index == -1) {
                // no session w that name
                response.type = JN_NAK;
                strcpy(response.data, "Session DNE");
            }
            else if (special_client->session[0] != '\0') {
                // already in session cant join rn
                // just checking first elem
                response.type = JN_NAK;
                strcpy(response.data, "Already in a session");
            }
            else if (sessions[session_index].client_count >= MAX_CLIENTS_PER_SESSION) {
                // here maxxes out clients
                response.type = JN_NAK;
                strcpy(response.data, "Session full");
            }
            else {
                // final case hopefully we can actually join

                // setting up client indo basically the client index is the number of client currently in its session
                // seeing the client array element with the client to add it into the array
                // updating the count for next time
                int client_index = sessions[session_index].client_count;
                sessions[session_index].s_clients[client_index] = special_client;
                sessions[session_index].client_count++;


                strcpy(special_client->session, sessions[session_index].session_id);

                response.type = JN_ACK;
                strcpy(response.data, "Successfully joined session");
                printf("oined session %s\n", special_client->session);
            }

            
            pthread_mutex_unlock(&sessions_mutex);

            response.size = strlen(response.data);
            char buffer[BUFFER_SIZE];
            serialize_msg(&response, buffer, BUFFER_SIZE);
            send(special_client->socket, buffer, strlen(buffer), 0);

            

        /***************************************************** 4. LEAVE A SESSION  *********************************************************/
        } else if (msg.type == LEAVE_SESS) {
            // leave session
            // STEP 1: lock the sessions database
            // STEP 2: Retrieve the actual index of the ID using the sesion name
            // STEP 3: in the session, look through the list of arrays, adn find the client again in the sessions array
            // STEP 4: If the client is last in the session delete the whole session
            
            // once we find the thing in the sessions array we can removce it from the actual dessions array

            pthread_mutex_lock(&sessions_mutex);

            int session_index = -1;
            for (int i = 0; i < MAX_SESSIONS; i++) {
                if (sessions[i].client_count > 0 && strcmp(sessions[i].session_id, special_client->session) == 0) {
                    session_index = i;
                    break;
                }
            }

            // creating teh msggg
            struct message response;
            memset(&response, 0, sizeof(response));
            strcpy(response.source, "server");

            // finding the stuff :(
            if (session_index == -1) {
                // basically if someone trying to leave but not in a session
                response.type = JN_NAK;
                strcpy(response.data, "Not in a session");
            } else {
                // check all the clients of the current session

                for (int curr_client = 0; curr_client < sessions[session_index].client_count; curr_client++) {

                    // if the current one is the client then remove it
                    if (sessions[session_index].s_clients[curr_client] == special_client) {

                        // just in this case we are going to shift the remaining clients up 

                        for (int shift = curr_client; shift < sessions[session_index].client_count - 1; shift++) {
                            sessions[session_index].s_clients[shift] = sessions[session_index].s_clients[shift + 1];
                        }

                        sessions[session_index].client_count--;

                        break;

                    }

                }

                // we check if the client count for teh session became 0 so we can delete it
                if (sessions[session_index].client_count == 0) {

                    // full reset the session 
                    memset(&sessions[session_index], 0, sizeof(session_t)); 
                    printf("Last client has left, no more sessions!\n", special_client->session);
                }

                // finally full clear the memory for that clients sesion
                memset(special_client->session, 0, MAX_NAME);

                // preparing end respondes
                response.type = JN_ACK;
                strcpy(response.data, "Successfully left session");
                printf("left session %s\n", special_client->session);

            }

            pthread_mutex_unlock(&sessions_mutex);

            // finally sending the response data to the user
            response.size = strlen(response.data);
            char buffer[BUFFER_SIZE];
            serialize_msg(&response, buffer, BUFFER_SIZE);
            send(special_client->socket, buffer, strlen(buffer), 0);


            
        /***************************************************** 5. CREATE A NEW SESSION  ****************************************************/
        } else if (msg.type == NEW_SESS) {
            // new session

            // STEP 1: Start by checking if it already exists (idk what we should do if it does exist)
            // STEP 2: If it doesnt afind the next available session
            // STEP 3: Create the new session

            pthread_mutex_lock(&sessions_mutex);

            // one is for checking if session exists other is vool fr checking if we actually found space
            int session_exits = 0;
            int session_index = -1;
            
            // Check if the session already exists
            for (int i = 0; i < MAX_SESSIONS; i++) {
                if (strcmp(sessions[i].session_id, msg.data) == 0) {
                    // theres already a session w this name
                    session_exits = 1;
                    break;
                }
                if (sessions[i].client_count == 0 && session_index == -1) {
                    // we found a place for teh session
                    session_index = i;
                }
            }

            // setting up response message
            struct message response;
            memset(&response, 0, sizeof(response));
            strcpy(response.source, "server");

            // ok now we handle the three cases

            if (session_exits) {
                // session exists doing an error for now saying exists try joining instead

                response.type = JN_NAK;
                strcpy(response.data, "Sorry! Session already exists");
                response.size = strlen(response.data);

            }
            else if (session_index > -1) {
                // we actually create a new session here
                // we put in the session id name, change the client count, add the client 
                // to the client list, then change the clients session to the 
                // new cession we just made
                strcpy(sessions[session_index].session_id, msg.data);
                sessions[session_index].client_count = 1;
                sessions[session_index].s_clients[0] = special_client;
                strcpy(special_client->session, msg.data);

                // setting up the response info stuff
                response.type = NS_ACK;
                strcpy(response.data, "Session created successfully!");

                // just like a rpint check
                printf("Session %s created\n", msg.data);

            }
            else {
                // we have no space send like a failure msg

                response.type = JN_NAK;
                strcpy(response.data, "Sorry! Maxxed num of sessions");
                response.size = strlen(response.data);

            }

            pthread_mutex_unlock(&sessions_mutex);

            // sending the message
            char buffer[BUFFER_SIZE];
            serialize_msg(&response, buffer, BUFFER_SIZE);
            send(special_client->socket, buffer, strlen(buffer), 0);

            
        /***************************************************** 6. SENDING A MESSAGE  ******************************************************/
        } else if (msg.type == MESSAGE) {
            // message
            
            // Step 1: basically retrieve the session of the current client
            // Step 2: Parse through all the users and send them each a message 
            // Step 3: keep in mind your going to have to use that specific users SOCK

            pthread_mutex_lock(&sessions_mutex);

            // finding the session
            int session_index = -1;

            for (int i = 0; i < MAX_SESSIONS; i++) {
                if (strcmp(sessions[i].session_id, special_client->session) == 0) {
                    session_index = i;
                    break;
                }
            }

            // if not found then the user isnt currently in a session
            if (session_index == -1) {
                printf("Error: Session not found for client %s.\n", special_client->id);

                // need to send an error message 
                struct message error_msg;
                memset(&error_msg, 0, sizeof(error_msg));

                // theres no like error message associated with the error so ill just send it in a message back
                error_msg.type = MESSAGE;
                strcpy(error_msg.source, "server");
                strcpy(error_msg.data, "Not in a Session");
                error_msg.size = strlen(error_msg.data);

                // serializing adn sending error message
                char error_buffer[BUFFER_SIZE];
                serialize_msg(&error_msg, error_buffer, BUFFER_SIZE);
                send(special_client->socket, error_buffer, strlen(error_buffer), 0);


                pthread_mutex_unlock(&sessions_mutex);
                return;
            }

            // technically we can send the bugger directly since already type of message 
            serialize_msg(&msg, buffer, BUFFER_SIZE); 

            // actually going through and sending the message to each client in the curent session
            for (int curr_client = 0; curr_client < sessions[session_index].client_count; curr_client++) {
                // send the data message to all clients 
                client_t *receiver = sessions[session_index].s_clients[curr_client];

                // Skipped the sender dont know is we need to go this
                if (receiver->socket != special_client->socket) {
                    send(receiver->socket, buffer, strlen(buffer), 0);
                }
            }

            pthread_mutex_unlock(&sessions_mutex);


        }
        else if (msg.type == QUERY) {
            
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
-  // oh no

*/
