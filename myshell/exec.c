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

char ** get_args(struct file * file) {
    char ** args = malloc( (file->size+1) * sizeof(char *));
    for(int i = 0; i<file -> size; i++) {
        args[i] = file -> args[i];
    }
    args[file->size] = NULL;
    return args;
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

        close(pipefd[1]); // Close unused write end of the pipe

        dup2(pipefd[0], STDIN_FILENO);
        
        //Redirect STDOUT if file2 has an output other than STDOUT.
        int fd_out = STDOUT_FILENO;
        if(file2 -> output != NULL) {
            printf("Program '%s' has output to file '%s'\n", file2->name, file2 -> output);
            fd_out = open(file2 -> output, O_WRONLY);
            if(fd_out < 0) {
                perror("Could not open output file");
                return;
            }
            dup2(fd_out, STDOUT_FILENO); //designate fd_out as the same file descriptor as STDOUT ; i.e., fd_out is now the output file.
            close(fd_out);
        }

        //fd_out can be refered as the output for this process.
        char ** args = get_args(file2);
        execv(file2 -> name, args);

        perror("Exec failed"); //exec will never return if successful
        return;
    }
    else {
        //parent process, execute file1
        close(pipefd[0]); // Close unused read end of the pipe

        dup2(pipefd[1], STDOUT_FILENO);


        //Redirect STDIN if file1 has an input other than STDOUT
        int fd_in = STDIN_FILENO;
        if(file1 -> input != NULL) {
            fd_in = open(file1 -> input, O_RDONLY);
            if(fd_in < 0) {
                perror("Could not open input file");
                return;
            }
            dup2(fd_in, STDIN_FILENO); //designate fd_in as the same file descriptor as STDIN ; i.e., fd_in is now the intput file.
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
void execute_file(struct file * file) {
    //Redirect STDIN if file has an intput other than STDIN.

    int fd_in = STDIN_FILENO;
    if(file -> input != NULL) {
        fd_in = open(file -> input, O_RDONLY);
        if(fd_in < 0) {
            perror("Could not open input file");
            return;
        }
        dup2(fd_in, STDIN_FILENO); //designate fd_in as the same file descriptor as STDIN ; i.e., fd_in is now the intput file.
    }

    //Redirect STDOUT if file has an output other than STDOUT.
    int fd_out = STDOUT_FILENO;
    if(file -> output != NULL) {
        fd_out = open(file -> output, O_WRONLY);
        if(fd_out < 0) {
            perror("Could not open output file");
            return;
        }
        dup2(fd_out, STDOUT_FILENO); //designate fd_out as the same file descriptor as STDOUT ; i.e., fd_out is now the output file.
    }

    char ** args = get_args(file); //change this !! Args will be malloced and must be freed, but execv never returns (so we can't ever free it). Solution : Reference file -> args as the paramter to exeecv. This means we must have file -> args have a NULL paramtere at the tail end. This can be done inside clean() within the parser file.
    execv(file -> name, args);
}
void exec_file(struct file * file) {
    pid_t pid;
    int status;

    pid = fork();
    if(pid < 0) {
        perror("Fork failed");
    }
    else if(pid == 0) { //Child process for executing a program. Execv does not return, so we must create a process which can be consumed by the aether.
        execute_file(file);
    }
    else { //parent process
        waitpid(pid, &status, 0);
    }
}
/**
 * @brief This function determines which indecies of the given command refer to a file which is indeed a program, and returns it.
 * 
 * e.g.
 * 
 * int * arr = determine_program_indecies(command);
 * arr[0] //the first program
 * arr[1] //the second program
 * 
 * @param command 
 * @return int *  -- guaranteed to be of length 2. 
 */
int * deteremine_program_indecies(struct command * command) {
    int * arr = malloc(2 * sizeof(int));
    arr[0] = -1;
    arr[1] = -1;

    int index = 0;

    for(int i = 0; i<command -> size; i++) {
        struct file * file = command -> files[i];
        if(access(file -> name, X_OK) == TRUE) {
            //the file is executable, and is thus a program
            arr[index] = i;
            index++;
            printf("A executable program is '%s'\n", file -> name);
        }
    }
    return arr;
}
int execute(struct command * command) {
    
    printf("Execute\n");

    int * arr = deteremine_program_indecies(command);
    if(arr[0] >= 0) {
        printf("The first program index is '%d' and is the program '%s'\n", arr[0], command -> files[arr[0]] -> name);
    }
    if(arr[1] >= 0) {
        printf("The second program index is '%d' and is the program '%s'\n", arr[1], command -> files[arr[1]] -> name);
    }

    if(arr[0] >= 0 && arr[1] >= 0) { //two programs, we are piping!
        struct file * file1 = command -> files[arr[0]];
        struct file * file2 = command -> files[arr[1]];
        exec_pipe(file1, file2);
    }
    else if(arr[0] >= 0) { //one executable program
        struct file * file1 = command -> files[arr[0]];
        exec_file(file1);
    }
    else {
        //no executable program
        if(command -> size > 0) {
            struct file * file = command -> files[0];
            if(strcmp(file -> name, "exit") == 0) {
                return EXIT_SUCCESS;
            }
        }
    }
    printf("Exiting execute()...\n");
    free(arr);
    return -1;
}