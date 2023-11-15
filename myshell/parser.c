/**
 * @brief This file contains the command line parser.
 */
#include <stdio.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

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
 * @brief Tokenizes the buffer into the command struct. 
 * 
 * @param buffer 
 * @param size 
 * @param command 
 */
void tokenize(char * buffer, size_t size, struct command * command) {
    for(int i = 0; i<size; i++) {

    }
}

/**
 * @brief Inserts the given filename into the command struct.
 * If I_RDIR is NON-NULL, then the file is meant to have input redirection from I_RDIR
 * If O_RDIR is NON-NULL, then the file is meant to have output redirection to be to O_RDIR
 * 
 * @param filename 
 * @param command 
 * @param I_RDIR 
 * @param O_RDIR 
 */
void insert(char * filename, struct command * command, char * I_RDIR, char * O_RDIR) {

    struct file * my_file = malloc(sizeof(struct file));
    my_file -> name = filename;
    my_file -> input = I_RDIR;
    my_file -> output = O_RDIR;

    command -> files [command -> size] = my_file; //insert the file into the command
    (command -> size)++;                          //incremenet the size of files within command
}

/**
 * @brief Obtains and returns the previous file specified within buffer which is less than i.
 * 
 * If the previous token is not a valid file, this method returns null.
 * 
 * @param buffer 
 * @param i 
 * @return char* 
 */
char * previous(char * buffer, int i, size_t size) {
    char * file = malloc(size * sizeof(char));
    //March backwards from buffer[i] and continuously append each character into file
    //Once you reach a whitepsace, you are done tokenizing that particular file.
}

/**
 * @brief Obtains and returns the next file specified within the buffer, which is strictly larger than i and less than size.
 * 
 * If a file is not valid within those parameters, this function returnsn null.
 * 
 * @param buffer 
 * @param i 
 * @param size 
 * @return char* 
 */
char * next(char * buffer, int i, size_t size) {
    //March forwards from buffer[i+1], continously appending each charater seen into a string. Once you reach whitespace, you are done.
    i++; //We want the next element starting at i+1
    char * file = malloc((size-i) * sizeof(char));
    int file_index = 0;
    while(i < size) {
        char c = buffer[i];
        switch(c) {
            case ' ' : {
                file[file_index] = '\0';
                return file;
            }
            case '>' : {
                file[file_index] = '\0';
                return file;
            }
            case '<' : {
                file[file_index] = '\0';
                return file;
            }
            case '|' : {
                file[file_index] = '\0';
                return file;
            }
            default : {
                file[file_index] = c;
                file_index++;
            }
        }
    }

    return NULL; //invalid (or maybe not?)


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
    
    char * s = malloc(size * sizeof(char));

    int s_index = 0;

    char * I_RDIR = NULL; //Input redirection
    char * O_RDIR = NULL; //Output redirection

    for(int i = 0; i<size; i++) {
        char c = buffer[i];
        switch(c) {
            case ' ' : { //white space separates files
                s[s_index] = '\0';
                insert(s, command, I_RDIR, O_RDIR);
                I_RDIR = NULL;
                O_RDIR = NULL;
                s = malloc(size * sizeof(char));
                s_index = 0;
                break;
            }
            case '<' : { //Input redirection
                I_RDIR = previous(buffer, i);
                break;
            }
            case '>' : { //Output redirection
                O_RDIR = next(buffer, i, size);
                break;
            }
            default : {
                s[s_index] = c;
                s_index++;
            }
        }
    }
}