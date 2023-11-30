#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include "parse.h"
#include <regex.h>
#include <dirent.h>
#include "exec.h"
#include <fcntl.h>
#include "myshell.h"
#include <sys/stat.h>
#include <strings.h>


#define INITIAL_BUFFER_SIZE 1024
#define BUFFER_INCREMENT 1024

char init_dir[PATH_MAX];


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

char * inital_directory(void) {
    return init_dir;
}

char * extract_filename(char * path) {
    if (path == NULL) {
        return NULL;
    }
    char* filename = strrchr(path, '/'); // Find the last occurrence of '/'
    if (filename == NULL) {
        // If no '/' found, return the original path
        return path;
    }

    // Move the pointer to the character after the last '/'
    return filename + 1;
}
int unix_command(char * inputPath) {
    if(index(inputPath, '/') != NULL) {
        const char * directories[] = { "/usr/local/bin/", "/usr/bin/", "/bin/" };
        char * filename = extract_filename(inputPath);
        for (int i = 0; i < 3; ++i) {
            DIR * dir = opendir(directories[i]);
            if (dir == NULL) {
                perror("opendir");
                return -1; // Error occurred while opening directory
            }

            struct dirent * entry;
            while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, filename) == 0) {
                    closedir(dir);
                    return TRUE; // Path matches a file in the directory
                }
            }
            closedir(dir);
        }
    }

    return FALSE; // No matching file found in the directories
}
void free_unix_commands(struct command * command) {
    for(int i = 0; i < command -> size; i++) {
        struct file * file = command -> files[i];
        if(unix_command(file -> name)) {
            free(file -> name);
        }
    }
}

int processBuffer(char *buffer, int exit_status) {
    if(TESTING) {
        printf("Buffer is '%s'\n", buffer);
    }
    struct command * command = parse(buffer);
    if(command != NULL) {
        exit_status = execute(command, exit_status);
        free_unix_commands(command);
        free_struct_command(command);
        if(exit_status == EXIT_FAILURE || exit_status == EXIT_SUCCESS) {
            printf("Exiting ... \n");
        }
        return exit_status;
    }
    else {
        return FAILURE;
    }

}

void resetBuffer(char **buffer, int *totalRead, int *bufferSize) {
    free(*buffer);
    *buffer = (char *)malloc(INITIAL_BUFFER_SIZE * sizeof(char));
    if (*buffer == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    (*buffer)[0] = '\0'; // Ensure the buffer starts with a null terminator
    *totalRead = 0;
    *bufferSize = INITIAL_BUFFER_SIZE;
}

void batch(char * filename, int exit_status) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;
    }

    char *buffer = (char *)malloc(INITIAL_BUFFER_SIZE * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        close(file);
        return;
    }
    buffer[0] = '\0'; // Initialize buffer with a null terminator

    ssize_t bytesRead;
    int totalRead = 0;
    int bufferSize = INITIAL_BUFFER_SIZE;
    char currentChar;
    while ((bytesRead = read(file, &currentChar, 1)) > 0) {
        if(exit_status == EXIT_FAILURE || exit_status == EXIT_SUCCESS) {
            return;
        }
        if (currentChar != '\n') {
            if (totalRead >= bufferSize - 1) {
                bufferSize += BUFFER_INCREMENT;
                buffer = realloc(buffer, bufferSize * sizeof(char));
                if (buffer == NULL) {
                    fprintf(stderr, "Memory allocation error\n");
                    close(file);
                    return;
                }
            }
            buffer[totalRead++] = currentChar;
        } else {
            buffer[totalRead] = '\0'; // Ensure the buffer ends with a null terminator
            exit_status = processBuffer(buffer, exit_status);
            resetBuffer(&buffer, &totalRead, &bufferSize);
        }
    }

    if (totalRead > 0) {
        buffer[totalRead] = '\0'; // Ensure the last line ends with a null terminator
        exit_status = processBuffer(buffer, exit_status);
    }

    free(buffer);
    close(file);
}


int main(int argc, char *argv[]){
    getcwd(init_dir, sizeof(init_dir));
    int exit_status = SUCCESS;
    if (argc > 1) {
        // Batch mode
        batch(argv[1], exit_status);
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
                exit_status = FAILURE;
            }
            else {
                traverse_command(command);

                exit_status = execute(command, exit_status);
                free_unix_commands(command);
                free_struct_command(command);
                if(exit_status == EXIT_FAILURE || exit_status == EXIT_SUCCESS) {
                    printf("Exiting ... \n");
                    return exit_status;
                }
            }
            memset(user_input, 0, sizeof user_input);
            // Set up next iteration
            printf("mysh> ");
        }
    }
    return 0;
}