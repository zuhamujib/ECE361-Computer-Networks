#ifndef MESSAGE_H  // If MESSAGE_H is not defined
#define MESSAGE_H  // Define MESSAGE_H

#define MAX_NAME 50  // Define appropriate limits
#define MAX_DATA 1024 // Define max data size
#define BUFFER_SIZE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "session.h"

struct message {
    unsigned int type;  // Message type
    unsigned int size;  // Data size
    unsigned char source[MAX_NAME];  // Sender's ID
    unsigned char data[MAX_DATA];  // Message content
};

// Serialization functions
void serialize_msg(struct message *msg, char *buffer, size_t buffer_size);
void deserialize_msg(const char *buffer, struct message *msg);

#endif  // End of MESSAGE_H