#include "clients.h"

#define hardcoded_users "users.txt"

void users_on_file() {
    FILE *fptr;

    // opening the file to read its contents
    fptr = fopen("hardcoded_users", "r");

    // Store the username and password
    user_t [100];

    // Read the content and store it inside myString
    fgets(myString, 100, fptr);

    // Print the file content
    printf("%s", myString);

    // Close the file
    fclose(fptr); 

}


void save_to_file(const char* username, const char* password);