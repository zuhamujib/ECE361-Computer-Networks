#include "clients.h"

#define hardcoded_users "users.txt"

user_t *users = NULL;
int user_count = 0;
int user_capacity = 10;

void load_users_on_file() {
    FILE *fptr;

    // opening the file to read its contents
    fptr = fopen(hardcoded_users, "r");
    if (!fptr) {
        perror("Error opening file");
        return;
    }

    // Store the username and password
    char username[MAX_NAME], password[MAX_NAME];

    // allocate memory for the users array
    users = malloc(sizeof(user_t) * user_capacity);

    while (fscanf(fptr, "%s %s", username, password) == 2) {
        // reallocating the size of the array if capacity has been reached
        if (user_count >= user_capacity){
            user_capacity *= 2;
            users = realloc(users, sizeof(user_t) * user_capacity);
        }

        // copying the contents of the file into the array
        strcpy(users[user_count].username, username);
        strcpy(users[user_count].password, password);
        user_count++;
    }

    // close the file
    fclose(fptr); 

}


void save_to_file(const char* username, const char* password) {
    FILE *fptr;
    // opening the file to read its contents
    fptr = fopen(hardcoded_users, "a");
    if (!fptr) {
        perror("Error opening file");
        return;
    }

    if (fptr) {
        fprintf(fptr, "\n%s %s", username, password);
        fclose(fptr);
    }
}