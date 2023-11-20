#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include "parser.h"
#include <regex.h>
#include <dirent.h>
#include "which.h"

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

        while (fgets(user_input, 1000, stdin) != NULL) {

            
            // strip newline from command
            user_input[strcspn(user_input, "\n")] = 0;

            // Check for cd.*$

            regex_t regex;
            int regex_val;

            /* Compile regular expression */
            regex_val = regcomp(&regex, "^cd.*", 0);

            // check
            if (regex_val) {
                fprintf(stderr, "Could not compile regex\n");
            }

            regex_val = regexec(&regex, user_input, 0, NULL, 0);

            // Check for builtin commands

            if ( strcmp("exit", user_input) == 0 || regex_val == 0) {
                break;
            }
            //pwd
            if ( strcmp("pwd", user_input) == 0 || regex_val == 0) {
                // detect command
                int builtin_to_run;
                // run pwd
                if ( strcmp("pwd", user_input) == 0 ){
                    pwd_builtin();
                }
                // run cd
                if ( regex_val == 0 ) {

                    // We have reached a cd function. we need to conduct a few checks
                    // create a copy of user input
                    char throwaway_input[5120];
                    strncpy(throwaway_input, user_input, 5120);

                    // Now set counter
                    int num_tokens = 0;

                    // Set token
                    char* token = strtok(throwaway_input, " ");
                    while (token != NULL) {
                        num_tokens++;
                        token = strtok(NULL, " ");
                    }

                    if (num_tokens == 2) {
                        // Now we tokenize until we get the last token
                        char *directory = strrchr(user_input, ' ');
                        // get last token
                        directory = directory +1;
                        cd_builtin(directory);
                    }
                }
            }
            else if(valid_which(user_input)) {
                //Guarantee ourselves that input is indeed 'which ' at the beginning
                which(user_input);
            }
            else {
                printf("command initiated: %s\n", user_input);
            }
            memset(user_input, 0, sizeof user_input);

            // Set up next iteration
            printf("mysh> ");
        }
   }
    return 0;
}