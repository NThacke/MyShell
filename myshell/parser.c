/**
 * @brief This file contains the command line parser.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
     * @brief The name of this file.
     */
    char * name;
    /**
     * @brief The arguments to the file.
     */
    char ** args;
    /**
     * @brief The number of arguments to the file.
     */
    int size;
    /**
     * @brief The redirected output of this file. NULL indicates STDOUT
     */
    char * output;
    /**
     * @brief The redirected input of this file. NULL indicates STDIN
     */
    char * input;
};
/**
 * @brief This struct is used to assist with tokenizing files. When tokenizing, this struct gives important information that is useful for the parser.
 * 
 */
struct token_helper {
    /**
     * @brief File which was tokenized
     * 
     */
    struct file * file;
    /**
     * @brief The next index to begin parsing.
     * 
     */
    int index;
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

void free_file_struct(struct file * file) {
    for(int i = 0; i<file->size; i++) {
        free(file->args[i]);
    }
    if(file -> input != NULL) {
        free(file -> input);
    }
    if(file -> output != NULL) {
        free(file -> output);
    }
    //file -> name is same as file -> args[0] by definition; no need to free it
    free(file->args);
    free(file);
}

void free_struct_command(struct command * c) {
    for(int i = 0; i<c->size; i++) {
        free_file_struct(c -> files[i]);
    }
    free(c->files);
    free(c);
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

void free_token_helper(struct token_helper * helper) {
    free_file_struct(helper -> file);
    free(helper);
}

/**
 * @brief Tokenizes a file from the buffer starting at the given index. Returns a token_helper struct which contains the file that was toknized as well as the index to next be parsed..
 * 
 * @param buffer 
 * @param size 
 * @param index 
 * @return struct token_helper * 
 */
struct token_helper * tokenizeFile(char * buffer, size_t size, int index) {
    printf("Tokenizing a file from buffer starting at index %d\n", index);
    
    struct token_helper * helper = malloc(sizeof(struct token_helper));
    struct file * file = malloc(sizeof(struct file));
    helper -> file = file;
    

    char ** args = malloc(size * sizeof(char *));

    int args_index = 0;
    
    char * word = malloc(size * sizeof(char));
    int word_index = 0;
    
    while(index < size) {
        char c = buffer[index];
        switch(c) {
            case ' ' : {
                //Insert the parsed word/argument into the list of arguments
                word[word_index] = '\0';
                word = realloc(word, word_index+1); //we now know the exact length
                args[args_index] = word; 
                printf("Parsed a token : '%s'\n", word);
                //Reset
                word_index = 0;
                word = malloc(size * sizeof(char));
                args_index++;

                break;
            }
            case '<' : {
                //finished parsing a file
                if(args_index > 0) {
                    file -> args = args;
                    file -> size = args_index;
                    file -> name = args[0];
                    helper -> index = index;
                    return helper;
                }
                free_token_helper(helper);
                free(args);
                free(word);
                return NULL; //args_index == 0; something weird happened.
            }
            case '>' : {
                //finished parsing a file
                if(args_index > 0) {
                    file -> args = args;
                    file -> size = args_index;
                    file -> name = args[0];
                    helper -> index = index;
                    return helper;
                }
                free_token_helper(helper);
                free(args);
                free(word);
                return NULL; //args_index == 0; something weird happened.
            }
            case '|' : {
                //finished parsing a file
                if(args_index > 0) {
                    file -> args = args;
                    file -> size = args_index;
                    file -> name = args[0];
                    helper -> index = index;
                    return helper;
                }
                free_token_helper(helper);
                free(args);
                free(word);
                return NULL; //args_index == 0; something weird happened.
            }
            default : {
                word[word_index] = c;
                word_index++;
            }
        }
        index++;
    }

    printf("Finished moving through the buffer. Args index is %d and word index is %d\n", args_index, word_index);

    if(word_index > 0) { //Edge case where the last token is end of line.

        //Insert the parsed word/argument into the list of arguments
        word[word_index] = '\0';
        word = realloc(word, word_index+1); //we now know the exact length
        args[args_index] = word; 
        printf("Parsed a token : '%s'\n", word);
        args_index++;

        file -> args = args;
        file -> size = args_index;
        file -> name = args[0];
        helper -> index = index;
    }
    //Finished parsing
    if(args_index == 0) {
        //no arguments; something weird happened
        printf("Args_index is 0\n");
        free_token_helper(helper);
        free(args);
        free(word);
        return NULL;;
    }
    
    return helper;

}  

void traverseFile(struct file * file) {
    printf("------------\n");
    for(int i = 0; i<file -> size; i++) {
        printf("'%s'\n", file -> args[i]);
    }
    printf("------------\n");   
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
    
    struct token_helper * helper = tokenizeFile(buffer, size, 0);

    if(helper != NULL) {
    traverseFile(helper -> file);
    free_token_helper(helper);
    }

    return NULL;
}