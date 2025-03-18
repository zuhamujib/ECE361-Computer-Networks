// needed to add this for the makefile
#include "message.h"

// functions to help serialize and deserialize with the delimter ":"
void serialize_msg(struct message *msg, char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "%u:%u:%s:%s", msg->type, msg->size, msg->source, msg->data);
    // puts msh into proper format
}

void deserialize_msg(const char *buffer, struct message *msg) {
     sscanf(buffer, "%d:%d:%49[^:]:%1023c", &(msg->type), &(msg->size), msg->source, msg->data);
     // retrieves the value, the size, the name up to 49 bytes, and the data up to 1023 bytes
}