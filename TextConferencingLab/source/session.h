#ifndef SESSION_H
#define SESSION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#define MAX_SESSIONS 10              // Maximum number of active sessions
#define MAX_CLIENTS_PER_SESSION 10   // Maximum number of clients per session
#define MAX_NAME 50                  // Maximum length for session and client names

// Enum for session-related packet types
typedef enum {
    LOGIN,       
    LO_ACK,     
    LO_NAK,      
    EXIT,        
    JOIN,        
    JN_ACK,      
    JN_NAK,      
    LEAVE_SESS,  
    NEW_SESS,    
    NS_ACK,      
    MESSAGE,     
    QUERY,       
    QU_ACK      
} PacketType;

typedef struct {
    char id[MAX_NAME];
    int socket; // for comm between server and user possibly
    char session[MAX_NAME];
    int active;
    time_t last_active;
} client_t;

typedef struct {
    char session_id[MAX_NAME];
    client_t *s_clients[MAX_CLIENTS_PER_SESSION];
    int client_count;
} session_t;

// Global array to store active sessions
extern session_t sessions[MAX_SESSIONS];
extern int num_sessions;  // Track the number of active sessions

#endif  // SESSION_H

