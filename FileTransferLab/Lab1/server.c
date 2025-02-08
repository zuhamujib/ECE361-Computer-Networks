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
#include <stdbool.h>
#include "packet.h"

int main(int argc, char *argv[]) {

    // STEP 1: TAKE IN THE LISTEN PORT
    // input should be UDP listen port 
    if (argv[1]==NULL) {
        perror("NO PORT PROVIDED\n");
        exit(1);
    }

    char* listen_port = argv[1];
    int port_value = atoi(listen_port);

    if (port_value < 0) {
        perror("Invalid port number\n");
        exit(1);
    }
    // STEP 2 MAKE THE SOCKET
    int sockfd;
    
    // From Beej's book section 5.3
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("SOCKET ERRORS\n");
        exit(1);
    }

    // STEP 3: BIND THE SOCKET
    struct sockaddr_in s_address;
    memset(&s_address, 0, sizeof(s_address));
    s_address.sin_family = AF_INET;
    s_address.sin_addr.s_addr = htonl(INADDR_ANY); // cant just have sin+addr need to convert the struct to a 32 bit num
    s_address.sin_port = htons(port_value);
    printf("Using Port Number: %d\n", port_value);
    fflush(stdout);

    // From Beej's book for section 5.3
    if (bind(sockfd, (struct sockaddr *)&s_address, sizeof(s_address)) < 0) {
        perror("BIND FAILED\n");
        close(sockfd);
        exit(1);
    }

    // STEP 4: RECIEVE MSG
    // buffer for the message itself
    char buffer[1024];
    

    // STEP 5: PREPARING FILE INFORMATION 
    FILE *file = NULL;
    unsigned int next_frag = 1;
    unsigned int total_frags = 0;
    char current_filename[256] = {0};

    bool connection = false;

    char initial[1024]; 


    while (1) {


        // SETTING UP INITIAL CONNECTION
        if (!connection) {

            for (int i = 0; i < 1024; i++) {
                initial[i] = 0;
            }

            struct sockaddr_in c_address;
            socklen_t addr_len = sizeof(c_address);

            printf("Waiting for inital connection...");
            fflush(stdout);
            ssize_t status = recvfrom(sockfd, initial, sizeof(initial)-1, 0, (struct sockaddr *)&c_address, &addr_len);
            if (status < 0) {
                perror("RECVFROM FAIL\n");
                close(sockfd);
                exit(1);
            }

            initial[1024] = '\0'; // must end in terminating char to read eof
            // STEP 5: CREATE AND SEND MESSAGE
            char* send_message;
            if (strcmp(initial, "ftp") == 0) {
                send_message = "yes";
            }
            else {
                send_message = "no";
            }

            if (sendto(sockfd, send_message, strlen(send_message), 0, (struct sockaddr *)&c_address, addr_len) == -1) {
                perror("SENDTO FAIL\n");
                close(sockfd);
                exit(0);
                exit(1);
            } 

            printf("Connection Complete!\n");

            connection = true;
        }
    

        printf("READY TO RECIEVE PACKETS\n\n");
        fflush(stdout);
        // STEP 6: RETRIEVE OUR MSG FROM CLIENT
        for (int i = 0; i < 1024; i++) {
            buffer[i] = 0;
        }

        struct sockaddr_in c_address;
        socklen_t addr_len = sizeof(c_address);

        ssize_t status = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr *)&c_address, &addr_len);
        fflush(stdout);
        if (status < 0) {
            perror("RECVFROM FAIL\n");
            close(sockfd);
            exit(1);
        }

        printf("Packet Recieved! \n");
        fflush(stdout);


        // STEP 7: BEGIN PARSING OUR PACKET
        // dont forget to eventually error check this
        struct packet pkt;
        printf("-------------------------------------------------------------------------");
        printf("%s\n", buffer);
        printf("-------------------------------------------------------------------------");

        char* parse;
        char* packet_info = buffer;

        parse = strtok(packet_info, ":");
        pkt.total_frag = atoi(parse);

        printf("\nTOTAL PACKETS: %d\n", pkt.total_frag);

        parse = strtok(NULL, ":");
        pkt.frag_no = atoi(parse);

        printf("PACKET NUM:%d\n", pkt.frag_no);
        fflush(stdout);

        parse = strtok(NULL, ":");
        pkt.size = atoi(parse);

        printf("PACKET SIZE:%d\n", pkt.size);
        fflush(stdout);

        parse = strtok(NULL, ":");
        pkt.filename = malloc(strlen(parse)+1);
        memcpy(pkt.filename, parse, strlen(parse)+1);
        printf("FILENAME: %s\n", pkt.filename);
        fflush(stdout);
        
        parse = strtok(NULL, "\0");
        memcpy(pkt.filedata, parse, pkt.size);

        // STEP 8: FILE WRITING 
        // first we check if we have a new file, new file number or new filename
        if (pkt.frag_no == 1 || strcmp(current_filename, pkt.filename) != 0) {
            // if previous file exists or is open we close it 
            if (file) {
                fclose(file);
                printf("File %s closed. \n", current_filename);
            }

            memcpy(current_filename, pkt.filename, strlen(pkt.filename)+1);

            fflush(stdout);
            // create the file and start writing

            char filepath[100];
            sprintf(filepath, "temp/%s", current_filename);
            file = fopen(filepath, "ab");
            if (!file) {
                perror("ERROR CRETAING FILE");
                exit(1);
            }

            // update remaining important info
            total_frags = pkt.total_frag;
            if (pkt.total_frag > 1) {
                next_frag = 2;
            } 
            // the next frag we expect
        }
        
        printf("Expected PACKET NO: %d", next_frag);
        // check if the next expected packet
        if (pkt.frag_no == next_frag) {
            fwrite(pkt.filedata, 1, pkt.size, file);
            next_frag++;
        }
        else
         {
            printf("ORDER PROBLEM");
            exit(1);
         }

         // STEP 9: SEND ACK
        char send_message[20];
        snprintf(send_message, sizeof(send_message), "ACK:%u", pkt.frag_no);
        if (sendto(sockfd, send_message, strlen(send_message), 0, (struct sockaddr *)&c_address, addr_len) == -1) {
            perror("SENDTO FAIL\n");
            close(sockfd);
            exit(1);
        } 

        // checking if file need to be closed, and resseting all info
        if (pkt.frag_no == total_frags) {
            fwrite(pkt.filedata, 1, pkt.size, file);
            next_frag++;
            fclose(file);
            printf("File completed transferring!\n\n");
            connection = false;
            file = NULL;
            next_frag = 1;
        }

    }
    
    close(sockfd);
    return 0;
}


// Essentially 3 Steps:
// 1. Retrieve the message; retrieve and parse the incoming packets
// 2. determine if its a new packet using fragment number
    // if one is the packet number, create a new file for this 
    // if non one packet number, check if file exists
        // if files already exists append the packet to the new file
        // if file doesnt exist error 
// 3. If the message was sent send back an ACK


// Many possible errors and areas of concern:
//      parsing through to recieve file data correct?
//      sending ack as packet or is msg just fine
//      dont know if next packet number thing is correctly implemented
//      check all data setting
//      add in error checking
//      memcpy used correct?
//      MAJOR CONCERN: IF WE SEND A FILE THRU DELIVER AND WE OPNE IT IN SERVER ASSUMING 
//      IT DOESNT EXIST EVEN THO THE FILES ARE IN THE SAME FOLDER THIS WILL CAUSE SOME PROBLES FOR US
//      POSSIBLY NEED TO SEND IN FILE FORM DIFFERENT FOLDER WHEN TESTING
