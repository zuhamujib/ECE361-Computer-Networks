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


    while (1) {

    
        // STEP 6: RETRIEVE OUR MSG FROM CLIENT
        for (int i = 0; i < 1024; i++) {
            buffer[i] = 0;
        }

        struct sockaddr_in c_address;
        socklen_t addr_len = sizeof(c_address);

        printf("Starting to wait for packets...\n");
        fflush(stdout);


        ssize_t status = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr *)&c_address, &addr_len);
        fflush(stdout);
        if (status < 0) {
            perror("RECVFROM FAIL\n");
            close(sockfd);
            exit(1);
        }

        printf("\nPacket Recieved! \n");
        fflush(stdout);


        // STEP 7: BEGIN PARSING OUR PACKET
        // dont forget to eventually error check this
        struct packet pkt;
        sscanf(buffer, "%u:%u:%u:%255[^:]:", &pkt.total_frag, &pkt.frag_no, &pkt.size, pkt.filename);
        // the 225 essentually takes in the file name of a max 255 but also it will stop if it reaches a :

        // retrieveing the actual data itsef (the next colon)
        char *file_data = strrchr(buffer, ':') + 1;
        // this hopefully should get us the last instance of the colon and one past it to be the file data
        memcpy(pkt.filedata, file_data, pkt.size);

        // STEP 8: FILE WRITING 
        // first we check if we have a new file, new file number or new filename
        if (pkt.frag_no == 1 || strcmp(current_filename, pkt.filename) != 0) {
            // if previous file exists or is open we close it 
            if (file) {
                fclose(file);
                printf("File %s closed. \n", current_filename);
            }

            // copy new file name to our current file name variable
            strncpy(current_filename, pkt.filename, sizeof(current_filename)-1);

            // create the file and start writing
            file = fopen(current_filename, "wb");
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
            fclose(file);
            printf("File completed transferring!\n");
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
