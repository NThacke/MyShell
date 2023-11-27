#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include "parse.h"
#include <regex.h>
#include <dirent.h>
#include "which.h"
#include "exec.h"

#define TRUE 0
#define FALSE 1




/* Changes directory
 * TODO: ERROR TESTING!!
*/
void cd_builtin(char* directory) {
    // needs error checking
    chdir(directory);
}


/* prints present working directory */
void pwd_builtin() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}


int main(int argc, char *argv[]){
    if (argc > 1) {
        // Batch mode
        printf("Running in batch mode");
    }
    else {
        // Set a 5kb buffer which is equal to 5120 chars
        char user_input[5120];

        // Here we enter interactive mode
        // First command is free
        printf("mysh> ");

        char cwd[PATH_MAX];

        while (fgets(user_input, 1000, stdin) != NULL) {

            
            // strip newline from command
            user_input[strcspn(user_input, "\n")] = 0;
            getcwd(cwd, sizeof(cwd));
            
            struct command * command = parse(user_input);

            if(command == NULL) {
                printf("Command not recognized\n");
            }
            else {
                traverse_command(command);

                int value = execute(command);
                free_struct_command(command);
                if(value == EXIT_FAILURE || value == EXIT_SUCCESS) {
                    printf("Exiting...\n");
                    exit(EXIT_SUCCESS);
                }
            }
            memset(user_input, 0, sizeof user_input);
            // Set up next iteration
            printf("mysh> ");
        }
    }
    return 0;
}