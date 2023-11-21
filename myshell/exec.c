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

/**
 * @brief Determines whether the file at (command->files[index]) is a pipe into another file. If so, this method returns 0 (TRUE), otherwise, returns 1 (FALSE).
 * 
 * @param command 
 * @param index 
 * @return int 
 */
int piping(struct command * command, int index) {
    //A pipe occurrs when one file's output is the next file's input.
    if(index < command -> size - 1) {

        struct file * current_file = command->files[index];
        struct file * next_file = command->files[index+1];
        printf("Determing if '%s' pipes into '%s'\n", current_file -> name, next_file -> name);

        char * output = current_file -> output;
        char * input = next_file -> input;

        printf("Output : '%s' | Input : '%s'\n", output, input);

        if(strcmp(output, next_file -> name) == 0 && strcmp(input, current_file->name) == 0 && access(current_file -> name, X_OK) != -1 && access(next_file -> name, X_OK) != -1) {
            //both current_file and next_file are executables (as determined by access()) furthermore, they both have the same input/output. Therefore, they must be two programs which have a pipe between them.
            return TRUE;
        }
    }
    return FALSE;

}

void execute_pipe(struct command * command, int index) {
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child process
        close(pipefd[1]);  // Close the write end of the pipe

        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin to read end of the pipe

        // Execute the second program
        struct file * second_program = command -> files[index+1];
        char ** args = get_args(second_program);
        printf("Executing second program\n");
        if(execv(second_program->name, args) == -1) {
            perror("execv");
            exit(EXIT_FAILURE);
        }
        printf("Finished second program\n");
    } else {  // Parent process
        close(pipefd[0]);  // Close the read end of the pipe

        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to write end of the pipe

        // Execute the first program
        struct file * first_program = command -> files[index];
        char ** args = get_args(first_program);
        printf("Executing first program\n");
        if(execv(first_program->name, args) == -1) {
            perror("execv");
            exit(EXIT_FAILURE);
        }
        printf("Finished first program\n");
    }
}

void execute_child(struct command * command, int index) {
    
    struct file * current_file = command -> files[index];
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
            if(piping(command, i) == TRUE) { //We are piping the current file into the next file; we must create two processes. This is different from redirection in this regard.
                printf("Recognized a pipe!\n");
                execute_pipe(command, i);
                i++; //we don't execute the next command, since it's a pipe
            }
            else {
                execute_child(command, i); //a better function name would be suitable; we are not executing children here.
            }
        }
    }
    printf("Exiting execute()...\n");
    return -1;
}