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
    s_address.sin_family #include <stdio.h>
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
    s_address.sin_family socklen_t addr_len = sizeof(c_address);

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

            printf("Connecsocklen_t addr_len = sizeof(c_address);

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

            printf("Connec--------------------");

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
        if (pkt.frag_no == 1 || strcmp(current_filename, pkt.filename) != 0) --------------------");

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
        if (pkt.frag_no == 1 || strcmp(current_filename, pkt.filename) != 0)    else
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
// 2. determine if its a new packet using fragment num   else
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
// 2. determine if its a new packet using fragment num#include <stdio.h>
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
    s_address.sin_family #include <stdio.h>
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
    s_address.sin_family #include <stdio.h>
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
    s_address.sin_family #include <stdio.h>
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
    s_address.sin_family #include <stdio.h>
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
    s_address.sin_family #include <stdio.h>
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
    s_address.sin_family 