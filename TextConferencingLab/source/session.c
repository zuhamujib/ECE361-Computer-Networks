#include "session.h"

// Array mapping packet type names to their enum values
// Define global variables here (so they exist only once)
session_t sessions[MAX_SESSIONS];  // Now properly defined in one file
int num_sessions = 0;

static struct {
    const char *name;
    PacketType type;
} packet_map[] = {
    {"LOGIN", LOGIN},
    {"LO_ACK", LO_ACK},
    {"LO_NAK", LO_NAK},
    {"EXIT", EXIT},
    {"JOIN", JOIN},
    {"JN_ACK", JN_ACK},
    {"JN_NAK", JN_NAK},
    {"LEAVE_SESS", LEAVE_SESS},
    {"NEW_SESS", NEW_SESS},
    {"NS_ACK", NS_ACK},
    {"MESSAGE", MESSAGE},
    {"QUERY", QUERY},
    {"QU_ACK", QU_ACK}
};

// Convert string to PacketType enum
PacketType get_packet_type(const char *type_str) {
    for (size_t i = 0; i < sizeof(packet_map) / sizeof(packet_map[0]); i++) {
        if (strcmp(packet_map[i].name, type_str) == 0) {
            return packet_map[i].type;
        }
    }
    return -1; // Invalid type
}

// Convert PacketType enum to string
const char* get_packet_type_string(PacketType type) {
    for (size_t i = 0; i < sizeof(packet_map) / sizeof(packet_map[0]); i++) {
        if (packet_map[i].type == type) {
            return packet_map[i].name;
        }
    }
    return "UNKNOWN"; // Handle invalid cases
}