/**
 * @brief This file contains the command line parser.
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief A parsed command from the command-line.
 * 
 */
struct command {
    /**
     * @brief A series of files which consist of the command.
     */
    struct file ** files;
    /**
     * @brief The number of files contained within files.
     */
    int size;
};

/**
 * @brief A file struct, which contains information regarding a file.
 * 
 * In particular, we are interested in the name of the file and whether or not the file has been redirected or piped.
 * 
 * Note : If a file has input redirection (and the associated file has output redirection to this specific file) then we can say the two files are PIPED together.
 * 
 *          In such a sense, we only need to signify input / output redirection. Piping is the definition of both of these operations.
 * 
 * For instance,
 * 
 * (1) foo > baz
 *  
 * (1) program foo will have its output be baz
 * 
 * (2) foo < baz
 * 
 * (2) program baz will have its input be from file foo
 * 
 * (3) foo | baz
 * 
 * (3) program foo will have its output be program baz, and program baz has its input from program foo
 * 
 */
struct file {
    /**
     * @brief The name of the file
     */
    char * name;
    /**
     * @brief The redirected output of this file
     */
    char * output;
    /**
     * @brief The redirected input of this file
     */
    char * input;
};

/**
 * @brief Creates a new struct command.
 * 
 * @return struct command* 
 */
struct command * new_command_struct() {
    struct command * c = malloc(sizeof(struct command));
    c -> files = malloc(sizeof(struct file *));
    c -> size = 0;
    return c;
}

void free_struct_command(struct command * c) {
    for(int i = 0; i<c->size; i++) {
        free(c -> files[i]);
    }
    free(c->files);
    free(c);
}

/**
 * @brief Parses the command-line, given in buffer (which contains size number of bytes).
 * 
 * @param buffer contains the command to be parsed
 * @param size the number of bytes in buffer
 * @return struct command * ; a command struct which contains information for the parsed command
 * 
 * Note : This current implementation does not support wildcards, and simply creates a command struct given a series of file names, redirection, or pipes.
 */
struct command * parse(char * buffer, size_t size) {
    
    struct command * command = new_command_struct();

    for(int i = 0; i<size; i++) {

    }
}