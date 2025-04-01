#include <stdio.h>
#include "message.h"

typedef struct {
    char username[MAX_NAME];
    char password[MAX_DATA];
} user_t;

extern user_t *users;
extern int user_count;
extern int user_capacity;

void load_users_on_file();
void save_to_file(const char* username, const char* password);