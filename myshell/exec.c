#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


#define TRUE 0
#define FALSE 1


/**
 * @brief Determines whether the given file refernces a built-in command, such as "pwd", "which", or "cd". Returns true (0) if any of them are true.
 * 
 * @param file 
 * @return int 
 */
int built_in(struct file * file) {
    char * name = file -> name;
    if(strcmp(name, "which") == 0) {
        return TRUE;
    }
    if(strcmp(name, "pwd") == 0) {
        return TRUE;
    }
    if(strcmp(name, "cd") == 0) {
        return TRUE;
    }
    if(strcmp(name, "exit") == 0) {
        return TRUE;
    }
    // return strcmp(name, "which") || strcmp(name, "pwd") || strcmp(name, "cd") || strcmp(name, "exit");
    return FALSE;
}
/**
 * @brief Executes the given file, which must be a built-in command.
 * 
 * @param file 
 */
int execute_built_in(struct file * file) {
    if(built_in(file) == 0) { //robust checking

        char * name = file -> name;
        if(strcmp(name, "exit") == 0) {
            printf("goodbye world\n");
            return EXIT_SUCCESS;
        }
    }
    return -1;
}
int input_file_descriptor(struct file * file) {
    if(file -> input == NULL) {
        return 0; //STDIN is designated as 0
    }
    else {

        int value = open(file->input, O_RDONLY);
        printf("The file desciptor for '%s' is '%d'\n", file -> input, value);
        return value;
    }
}

int output_file_descriptor(struct file * file) {
    if(file -> output == NULL) {
        return 1;
    }
    else {
        int value = open(file -> output, O_WRONLY);
        printf("The file desciptor for '%s' is '%d'\n", file -> output, value);
        return value;
    }
}

char ** get_args(struct file * file) {
    char ** args = malloc( (file->size+1) * sizeof(char *));
    for(int i = 0; i<file -> size; i++) {
        args[i] = file -> args[i];
    }
    args[file->size] = NULL;
    return args;
}
int execute(struct command * command) {
    
    printf("Execute\n");
    for(int i = 0; i<command -> size; i++) {
        
        struct file * current_file = command -> files[i];
        if(built_in(current_file) == 0) {
            printf("Built-in command recognized : '%s'\n", current_file -> name);
            int value = execute_built_in(current_file);
            if(value == EXIT_FAILURE || value == EXIT_SUCCESS) { //execute built in recognizes exit call; if so, exit the program completely.
                return value;
            }
        }
        else { //not built-in, create a new process to execute.
            pid_t pid;
            int status;

            pid = fork();

            if(pid == -1) {
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
            else if(pid == 0) { //We are in child process

            printf("In child process\n");

            int inputFile = input_file_descriptor(current_file);
            if(inputFile == -1) {
                perror("open input file");
                exit(EXIT_FAILURE);
            }
            int outputFile = output_file_descriptor(current_file);
            if(outputFile == -1) {
                perror("open output file");
                exit(EXIT_FAILURE);
            }

                printf("{Filename : '%s' | Input FD : '%d' | Output FD : '%d'}\n", current_file -> name, inputFile, outputFile);

                //We now have the input/output file descriptors for the current file.
                if(dup2(inputFile, 0) == -1) {
                    perror("dup2 input");
                    exit(EXIT_FAILURE);
                }
                // close(inputFile);


                if(dup2(outputFile, 1) == -1) {
                    perror("dup2 output");
                    exit(EXIT_FAILURE);
                }
                // close(outputFile);

                char ** args = get_args(current_file);

                printf("Invoking execv\n");
                if(execv(current_file -> name, args) == -1) {
                    perror("execv error");
                    free(args);
                    close(inputFile);
                    close(outputFile);
                    exit(EXIT_FAILURE);
                }

                free(args);
                close(inputFile);
                close(outputFile);
        }
        else {
             // Parent process
            waitpid(pid, &status, 0); // Wait for the child to finish
        }
    }
    }
    return -1;
}