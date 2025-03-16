#ifndef MESSAGE_H  // If MESSAGE_H is not defined
#define MESSAGE_H  // Define MESSAGE_H

#define MAX_NAME 50  // Define appropriate limits
#define MAX_DATA 1024  // Define max data size

struct message {
    unsigned int type;  // Message type
    unsigned int size;  // Data size
    unsigned char source[MAX_NAME];  // Sender's ID
    unsigned char data[MAX_DATA];  // Message content
};

#endif  // End of MESSAGE_H