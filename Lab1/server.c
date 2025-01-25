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

int main(int argc, char *argv[]) {

    // STEP 1: TAKE IN THE LISTEN PORT
    // input should be UDP listen port 
    if (argv[1]==NULL) {
        printf("NO PORT PROVIDED\n");
        exit(1);
    }

    char* listen_port = argv[1];
    int port_value = atoi(listen_port);

    //printf("successfully here\n");
    //fflush(stdout);

    // STEP 2 MAKE THE SOCKET
    int sockfd;
    
    // From Beej's book section 5.3
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("SOCKET ERRORS\n");
        exit(1);
    }

    // STEP 3: BIND THE SOCKET
    struct sockaddr_in s_address;
    memset(&s_address, 0, sizeof(s_address));
    s_address.sin_family = AF_INET;
    s_address.sin_addr.s_addr = htonl(INADDR_ANY); // cant just have sin+addr need to convert the struct to a 32 bit num
    s_address.sin_port = htons(port_value);
    printf("PORT VALUE: %d\n ", port_value);
    fflush(stdout);

    // From Beej's book for section 5.3
    if (bind(sockfd, (struct sockaddr *)&s_address, sizeof(s_address)) < 0) {
        printf("BIND FAILED\n");
        close(sockfd);
        exit(1);
    }

    // STEP 4: RECIEVE MSG
    // buffer for the message itself
    char buffer[1024];
    // clearing and setting buffer
    for (int i = 0; i < 1024; i++) {
        buffer[i] = 0;
    }

    

    struct sockaddr_in c_address;
    socklen_t addr_len = sizeof(c_address);


    printf("About to wait for message...\n");
    fflush(stdout);
    ssize_t status = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr *)&c_address, &addr_len);
    printf("here \n");
    fflush(stdout);
    if (status < 0) {
        printf("RECVFROM FAIL\n");
        exit(0);
    }

    buffer[1024] = '\0'; // must end in terminating char to read eof
    printf("message: %s\n", buffer);

    // STEP 5: CREATE AND SEND MESSAGE
    char* send_message;
    if (strcmp(buffer, "ftp") == 0) {
        send_message = "yes";
    }
    else {
        send_message = "no";
    }

    if (sendto(sockfd, send_message, strlen(send_message), 0, (struct sockaddr *)&c_address, addr_len) == -1) {
        printf("SENDTO FAIL\n");
    } 

    printf("Message Sent!");
    close(sockfd);

    printf("hello word\n");



    return 0;
}


// TO DO:
// - fixing print statments and error handling 
// 