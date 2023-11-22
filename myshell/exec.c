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

/**
 * @brief Determines and returns the number of processes which must be initated to execute the given command.
 * 
 * 
 * 
 * e.g.
 * 
 * (1) foo | bar > output.txt
 * (1) foo and bar are their own processes; we have 2 processes to execute. We do not process output.txt
 * 
 * (2) foo.txt < bar | baz > quux.txt
 * (2) The answer is 2. Bar and baz are their own processes. Bar has its input from foo.txt, and it's output into baz.
 * (2)                                                       Baz has its input from bar, and it's output into quux.txt
 * 
 * There cannot be more than two processes (as piping can only occur between two programs, and is not generalized here.)
 * 
 * Q : Is the following command valid?
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * (1) foo.txt > bar > baz.txt
 * 
 * (1) A : Yes. There is one process, bar, which has its input from foo.txt and sends its output to baz.txt
 *
 * (2) which > baz.txt | bar.txt
 * 
 * (2) A : No, this is invalid. It doesn't make sense to have a .txt file pipe into another .txt file
 * 
 * 
 * (3) which | bar > output.txt
 * 
 * (3) A : Yes, which and bar are their own processes.
 * 
 * (4) which > output.txt > output2.txt
 * 
 * (4) A : No, this doesn't make sense. Which can have its output be redirected into output.txt, but it doesn't make sense for output.txt to redirect its output to some other text file! It's not a program, it's just a text file that recieved output!
 * 
 * In summary, we only need to concern ourselves with at most 2 processes. It basically boils down to one process potentially piping into some other process. If there is no pipe, then there is only one process! It would be invalid to continuously redirect output (foo > bar > quux > baz doesn't mean anything -- you're redirecting foo's output into a **file**, bar, and there is no output of bar, since it's just a text file and not a program!)
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  
 * 
 * @param command 
 * @return int 
 */
int number_of_processes(struct command * command) {
    return -1;
}
void execute_pipe(struct file * file1, struct file * file2) {
    pid_t pid;
    int status;

    int pipefd[2];
    if(pipe(pipefd) < 0) { //invoke the system call pipe() to designate the read/write end as valid, open file-descroptiors.
        perror("Pipe failed");  //the OS could not designate a valid file descriptor for read / write
        return;
    }

    //We are going to create one process. The current process will handle file1, and the forked process will handle file2.
    pid = fork();
    if(pid < 0) {
        perror("Fork failed");
        return;
    }

    if(pid == 0) {
        //Redirect STDIN to the read end of the pipe. file2 is reading from file1.
        dup2(pipefd[0], STDIN_FILENO);
        
        //Redirect STDOUT if file2 has an output other than STDOUT.
        int fd_out = STDOUT_FILENO;
        if(file2 -> output != NULL) {
            fd_out = open(file2 -> output, O_WRONLY);
            if(fd_out < 0) {
                perror("Could not open output file");
                return;
            }
            dup2(fd_out, STDOUT_FILENO); //designate fd_out as the same file descriptor as STDOUT ; i.e., fd_out is now the output file.
        }

        //fd_out can be refered as the output for this process.
        char ** args = get_args(file2);
        execv(file2 -> name, args);

        perror("Exec failed"); //exec will never return if successful
        return;
    }
    else {
        //parent process, execute file1
        dup2(pipefd[1], STDOUT_FILENO);

        //Redirect STDIN if file1 has an input other than STDOUT
        int fd_in = STDIN_FILENO;
        if(file1 -> input != NULL) {
            fd_in = open(file2 -> input, O_RDONLY);
            if(fd_in < 0) {
                perror("Could not open input file");
                return;
            }
            dup2(fd_in, STDIN_FILENO);
        }
        char ** args = get_args(file1);
        execv(file1 -> name, args);

        perror("Exec failed");
        return;
    }
}

/**
 * @brief Executes a pipe between file1 and file2. This function should be invoked by the execute() function.
 * 
 * @param file1 
 * @param file2 
 */
void exec_pipe(struct file * file1, struct file * file2) {

    pid_t pid;
    int status;

    pid = fork();
    if(pid < 0) {
        perror("Fork failed");
    }
    else if(pid == 0) { //Child process for executing a pipe. Execv does not return, so we must create a process which can be consumed by the aether.
        execute_pipe(file1, file2);
    }
    else { //parent process
        waitpid(pid, &status, 0);
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
                exec_pipe(command -> files[i], command -> files[i+1]); //not general !! please fix me !!
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