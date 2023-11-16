/**
 * @brief This file contains the command line parser.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

/**
 * @brief This enum is used to signify when redirection occurs.
 * 
 * input   === '<'
 * output  === '>'
 * pipe    === '|'
 * nil     === none of the above
 * 
 */
enum redirection {
    input, output, pipe, nil
};
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
 * @brief A struct / object that contains wildcard components. In particular, the number of files which match the wildcard definition, as well as all of those components (files).
 * 
 */
struct wildcard_components {
    /**
     * @brief The wildcard files that match the given wildcard.
     */
    char ** wildcards;
    /**
     * @brief The number of wildcards components.
     */
    int size;
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
void traverseFile(struct file * file) {
    printf("------------\n");
    printf("File name : '%s'\n", file -> name);
    printf("File input : '%s'\n", file -> input);
    printf("File output : '%s'\n", file -> output);
    printf("File arguments ... \n");
    for(int i = 0; i<file -> size; i++) {
        printf("'%s'\n", file -> args[i]);
    }
    printf("------------\n");   
}

void free_file_struct(struct file * file) {
    if(file != NULL) {
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
}

void free_struct_command(struct command * c) {
    if(c != NULL) {
        for(int i = 0; i<c->size; i++) {
            free_file_struct(c -> files[i]);
        }
        free(c->files);
        free(c);
    }
}


void free_token_helper(struct token_helper * helper) {
    if(helper != NULL) {
        free_file_struct(helper -> file);
        free(helper);
    }
}

/**
 * @brief Creates a new file struct on the heap, with all entries as NULL (for pointers) and 0 (for integers).
 * 
 * @return struct file* 
 */
struct file * new_file_struct() {
    struct file * file = malloc(sizeof(struct file));
    file -> args = NULL;
    file -> input = NULL;
    file -> output = NULL;
    file -> size = 0;
    return file;
}

void traverse_command(struct command * command) {
    for(int i = 0; i<command -> size; i++) {
        traverseFile(command -> files[i]);
    }
}

/**
 * @brief Determines the wildcard string located at buffer[index], and returns it. Note that buffer[index] must be the '*' character.
 * 
 * @param buffer 
 * @param size 
 * @param index 
 * @return char* 
 */
char * wildcard_f(char * buffer, size_t size, int index) {
    if(buffer[index] == '*') {
        char * s = malloc(size * sizeof(char));

    }
    return NULL; //invalid
}
/**
 * @brief Tokenizes a file from the buffer starting at the given index. Returns a token_helper struct which contains the file that was toknized as well as the index to next be parsed.
 * 
 * @param buffer 
 * @param size 
 * @param index 
 * @return struct token_helper * 
 */
struct token_helper * tokenizeFile(char * buffer, size_t size, int index) {
    printf("Tokenizing a file from buffer starting at index %d\n", index);
    printf("Character at index %d is '%c'\n", index, buffer[index]);

    struct token_helper * helper = malloc(sizeof(struct token_helper));
    struct file * file = new_file_struct();
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
                if(word_index > 0) {
                    word[word_index] = '\0';
                    word = realloc(word, word_index+1); //we now know the exact length
                    args[args_index] = word; 
                    printf("Parsed a token : '%s'\n", word);
                    //Reset
                    word_index = 0;
                    word = malloc(size * sizeof(char));
                    args_index++;
                }
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
                break;
                // free_token_helper(helper);
                // free(args);
                // free(word);
                // return NULL; //args_index == 0; something weird happened.
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
                break;
                // free_token_helper(helper);
                // free(args);
                // free(word);
                // return NULL; //args_index == 0; something weird happened.
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
                break;
                // free_token_helper(helper);
                // free(args);
                // free(word);
                // return NULL; //args_index == 0; something weird happened.
            }
            case '*' : { //wildcard case
                char * wildcard = determine_wildcard(buffer, size, index);
                struct wildcard_components * wildcard_components = valid_wildcards(wildcard);
                //add all of the wildcard componenets as arguments to the current file
                
                for(int i = 0; i<wildcard_components->size; i++) {
                    args[args_index] = wildcard_components ->wildcards[i];
                    args_index++;
                }
                
                //reset
                index = next_space(buffer, size, index); //index goes to the next space character, as we do not want to parse the wildcard any further.

                word_index = 0; //we can overwrite word as we don't use it as a filename

                break;
            }   
            default : {
                word[word_index] = c;
                word_index++;
                break;
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
/**
 * @brief Inserts the file contained within helper into the command.
 * 
 * @param command 
 * @param helper 
 */
void insert(struct command * command, struct token_helper * helper) {
    printf("Inserting a new file into command\n");
    printf("Command currently has size %d \n", command->size);
    traverse_command(command);

    command -> files = realloc(command -> files, (command->size + 1) * sizeof(struct file *));

    command -> files[command -> size] = helper -> file;
    command -> size ++;
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

    struct file * previous = NULL;

    enum redirection redirect = nil;

    while(helper != NULL) {

        switch(redirect) { //piping is the same thing as redirection, it's just that piping is between programs and not literal files!
            case output : {
                helper -> file -> input = previous -> name;
                previous -> output = helper -> file -> name;
                break;
            }
            case input : {
                helper -> file -> input = previous -> name;
                previous -> output = helper -> file -> name;
                break;
            }
            case pipe : {
                helper -> file -> input = previous -> name;
                previous -> output = helper -> file -> name;
                break;
            }
            default : {
                //nothing
                break;
            }
        }






        previous = helper -> file;
        insert(command, helper);
        int index = helper->index;
        free(helper);
        
        switch(buffer[index]) {
            case '>' : { //redirect previous output to the next filename that appears
                redirect = output;
                index++;
                break;
            }
            case '<' : { //redirect input to next filename as the previous file
                redirect = input;
                index++;
                break;
            }
            case '|' : { //pipe two programs; previous output goes to next input
                redirect = pipe;
                index++;
                break;
            }
        }

        helper = tokenizeFile(buffer, size, index);
    }

    traverse_command(command);

    return command;
}