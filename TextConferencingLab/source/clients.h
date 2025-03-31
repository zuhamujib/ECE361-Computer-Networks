#include <stdio.h>
#include "message.h"

typedef struct {
    char* username[MAX_NAME];
    char* passsword[MAX_NAME];
} user_t;

extern users_t *users;
extern int user_count;

void users_on_file();
void save_to_file(const char* username, const char* password);