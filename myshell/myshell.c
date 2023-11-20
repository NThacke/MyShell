#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include "parser.h"
#include <regex.h>
#include <dirent.h>

#define TRUE 0
#define FALSE 1




/* Changes directory
 * TODO: ERROR TESTING!!
*/
void cd_builtin(char* directory) {
    // needs error checking
    chdir(directory);
}

/**
 * @brief Determines and returns the program which the command 'which' is referring to.
 * 
 * @param input Guaranteed to be atleast 5120 bytes long, as determiend by main()
 * @return char* malloced on the heap; must be freed
 */
char * which_program_name(char * input) {
    if(!(input[0] == 'w' && input[1] == 'h' && input[2] == 'i' && input[3] == 'c' && input[4] == 'h' && input[5] == ' ')) {
        return NULL; //invalid, not a which command!
    }
    char * name = malloc((5120-6)*sizeof(char));
    strcpy(name, (input+6)); //copy the name, which starts at index 6

    if(strlen(name) > 0) {
        return name;
    }
    else {
        free(name);
        return NULL;
    }
}

/**
 * @brief Searches in the given directory for the given program. If the program exists in the directory, then this function returns the path to the program (absolute). Otherwise, this function will return NULL.
 * 
 * @param program 
 * @param directory 
 * @return char* 
 */
char * search_directory(char * program, char * directory) {
    struct dirent * entry = NULL;
    DIR * dir = opendir(directory);
    if(dir != NULL) {
        while( (entry = readdir(dir)) != NULL) {
            if(strcmp(entry -> d_name, program) == 0) {
                return directory;
            }
        }
        closedir(dir);
    }
    return NULL;
}
/**
 * @brief Searches for the given program amongst the directories "/usr/local/bin", "/usr/bin", and "/bin", in that order. If the program name does not exist in any of those directories, this function will return NULL.
 * 
 * @param program 
 * @return char* 
 */
char * program_path(char * program) {

    char * path = NULL;

    if(path == NULL) {
        path = search_directory(program, "/usr/local/bin");
    }
    if(path == NULL) {
        path = search_directory(program, "/usr/bin");
    }
    if(path == NULL) {
        path = search_directory(program, "/bin");
    }

    if(path != NULL) {
        char * path_and_program = malloc((strlen(program) + strlen(path)+2) * sizeof(char));
        int index = 0;
        while(*path) {
            path_and_program[index] = *path;
            path++;
            index++;
        }

        path_and_program[index] = '/';
        index++;

        while(*program) {
            path_and_program[index] = *program;
            program++;
            index++;
        }

        return path_and_program;
    }
    return path;

}
/* prints which program and path
 * TODO: Entire function
*/
void which_builtin(char * input) {
    // Stub
    //We have guaranteed outselves that the beginning of input is indeed 'which '.
    //Now we must find the program that follows it.
    char * program_name = which_program_name(input);
    if(program_name != NULL) {
        // printf("Program name is '%s'\n", program_name);

        //Now we look for the program name.
        char * path = program_path(program_name);
        if(path == NULL) {
            printf("'%s' was not found.\n", program_name);
        }
        else {
            printf("%s\n", path);
            free(path);
        }
        free(program_name);
    }
    else {
        printf("Invalid program name given to 'which' command.\n");
    }
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

int which(char * input) {
    //We guarantee ourselves that the input is atleast 5120 bytes long
    return input[0] == 'w' && input[1] == 'h' && input[2] == 'i' && input[3] == 'c' && input[4] == 'h' && input[5] == ' ';
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
            else if(which(user_input)) {
                //Guarantee ourselves that input is indeed 'which ' at the beginning
                which_builtin(user_input);
            }
            else {
                printf("command initiated: %s\n", user_input);
            }
            memset(user_input, 0, sizeof user_input);

            // Set up next iteration
            printf("mysh> ");
        }
   }
}