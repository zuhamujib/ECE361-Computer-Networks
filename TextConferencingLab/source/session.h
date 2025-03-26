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
    LOGIN,       // Client logs into the server
    LO_ACK,      // Server acknowledges successful login
    LO_NAK,      // Server sends login failure reason
    EXIT,        // Client exits the server
    JOIN,        // Client joins an existing session
    JN_ACK,      // Server acknowledges successful session join
    JN_NAK,      // Server sends session join failure reason
    LEAVE_SESS,  // Client leaves a session
    NEW_SESS,    // Client creates a new session
    NS_ACK,      // Server acknowledges session creation
    MESSAGE,     // Client sends a message
    QUERY,       // Client requests a list of online users and active sessions
    QU_ACK       // Server responds with a list of users and sessions
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

PacketType get_packet_type(const char *type_str);
const char* get_packet_type_string(PacketType type);

#endif  // SESSION_H

