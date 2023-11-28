#include <stdio.h>
#include <stdlib.h>
#include "DLL.h"
#include <string.h>
#include <ctype.h>


#define TRUE 1
#define FALSE 0

#define TESTING TRUE

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

void traverse_command(struct command * command) {
    for(int i = 0; i<command -> size; i++) {
        traverseFile(command -> files[i]);
    }
}



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
    if(file != NULL) {
        for(int i = 0; i<file->size; i++) {
            printf("Freeing '%s' at address '%p'\n", file -> args[i], file -> args[i]);
            free(file->args[i]);
        }
        //file -> name is same as file -> args[0] by definition; no need to free it
        free(file -> name);
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

/**
 * @brief Creates a new file struct on the heap, with all entries as NULL (for pointers) and 0 (for integers).
 * 
 * @return struct file* 
 */
struct file * new_file_struct() {
    struct file * file = malloc(sizeof(struct file));
    file -> args = malloc(sizeof(char *));
    file -> input = NULL;
    file -> name = NULL;
    file -> output = NULL;
    file -> size = 0;
    return file;
}

/**
 * @brief Tokenizes the given buffer into a series of tokens. Note that the given buffer *must* have space-separated tokens. i.e., invoke the space_separate function prior to invocation of this function
 * 
 * @param buffer 
 * @param tokens 
 */
struct LinkedList * tokenize(char * buffer) {

    struct LinkedList * tokens = newLinkedList();

    int index = 0;

    int word_size = 256;
    char * word = malloc(word_size * sizeof(char));
    int word_index = 0;

    while(buffer[index] != '\0') {
        if(buffer[index] == ' ') { //finished tokenzing a token; We guarantee ourselves that tokens are separated by spaces

            word = realloc(word, (word_index+1) * sizeof(char));
            word[word_index] = '\0';
            add_tail(tokens, word);

            if(TESTING) {
                printf("Tokenized the token : '%s'\n", word);
            }

            //reset the word
            word_size = 256;
            word_index = 0;
            word = malloc(word_size * sizeof(char));
        }
        else { //we are still tokenizing a token
            if(word_index == word_size-1) { //if the token is too large; this is unlikely but not impossible
                word_size = word_size * 2;
                word = realloc(word, word_size * sizeof(char));
            }

            word[word_index] = buffer[index];
            word_index++;
        }
        index++;
    }

    free(word); //MEMORY LEAK FIX. word always got malloced at the end of a token, including the last token! We never freed it, until now.
    return tokens;
}

/**
 * @brief This enum is used to signify when redirection occurs.
 * 
 * input   === '<'
 * output  === '>'
 * pipe    === '|'
 * nil     === none of the above
 * 
 */
enum redirect {
    input, output, pipe, nil
};

void add_file(struct command * command, struct file * file) {
    command -> size ++;
    command -> files = realloc(command->files, (command->size) * (sizeof(struct file *)));
    command -> files[command -> size-1] = file;
}

void add_arg(struct file * file, char * arg) {
    file -> size ++;
    file -> args = realloc(file->args, (file -> size) * sizeof(char *));
    file -> args[file -> size -1] = arg;
}

char * file_name(char * path) {
    const char* fileName = path + strlen(path) - 1; // Start at the end of the path

    // Move backward until a directory separator or the beginning of the path is found
    while (fileName >= path && *fileName != '\\' && *fileName != '/') {
        fileName--;
    }

    // Move forward by one character to point at the filename
    fileName++;

    // Calculate the length of the filename
    size_t fileNameLength = strlen(fileName);

    // Allocate memory for the filename on the heap
    char* extractedFileName = (char*)malloc((fileNameLength + 1) * sizeof(char));
    if (extractedFileName == NULL) {
        printf("Memory allocation failed.\n");
        return NULL; // Return NULL if allocation fails
    }

    // Copy the filename to the newly allocated memory
    strcpy(extractedFileName, fileName);

    return extractedFileName;

}
int not_redirect(struct DLLNode * node) {
    return strcmp(node -> value, ">") != 0 && strcmp(node -> value, "<") != 0 && strcmp(node -> value, "|");
}
/**
 * @brief This function transforms the LinkedList of tokens into a struct command.
 * 
 * @param tokens 
 * @return struct command* 
 */
struct command * transform(struct LinkedList * tokens) {

    struct DLLNode * current_token = tokens -> head;

    if(current_token != NULL) {
        struct command * command = new_command_struct();


        struct file * current_file = new_file_struct();

        enum redirect redirect = nil;

        while(current_token != NULL) {

            if(current_file -> name == NULL) {
                current_file -> name = current_token -> value;
            }

            switch(redirect) {
                case input : {
                    //the current file has input from the current token
                    current_file -> input = current_token -> value;
                    break;
                }
                case output : {
                    //the current file has output to the current token
                    current_file -> output = current_token -> value;
                    break;
                }
                case pipe : {
                    //the current file is piping into the current token
                    current_file -> output = current_token -> value;

                    struct file * temp = current_file;
                    current_file = new_file_struct(); //once you say pipe, you are tokenzing a different file
                    current_file -> name = current_token -> value;
                    current_file -> input = temp -> name;

                    add_arg(temp, NULL);
                    add_file(command, temp);
                }
                default : {
                    //nothing; the current token is simply an argument to the current file
                    if(not_redirect(current_token)) {
                
                        if(strcmp(current_file -> name, current_token -> value) == 0) {
                            char * filename = file_name(current_file -> name);
                            add_arg(current_file, filename);
                            if(current_token -> value != current_file -> name) { //Since we added the argument /filename/ to the file struct, the current_token -> value will not be freed, and must be freed here. But only if the token and filename are not the same pointer.
                                free(current_token -> value);
                            }
                            current_token -> value = filename;
                        }
                        else {
                            add_arg(current_file, current_token -> value);
                        }
                    }
                }
            }

            if( strcmp(current_token -> value, "<") == 0) {
                redirect = input;
            }
            else if( strcmp(current_token -> value, ">") == 0) {
                redirect = output;
            }
            else if(strcmp(current_token -> value, "|") == 0) {
                redirect = pipe;
            }
            else {
                redirect = nil;
            }

            if(current_token -> next == NULL) {
                add_arg(current_file, NULL);
                add_file(command, current_file);
            }
            current_token = current_token -> next;        
        }
        return command;
    }
    return NULL;
}

/**
 * @brief Determines if the given list of tokens is indeed valid.
 * 
 * An invalid token is one of the following (not exhaustive) : 
 * 
 *  foo |
 *  < foo
 *  foo >
 *  
 * 
 *  Every redirecting token must have another, non redirecting token immediately following it and after it.
 * 
 * If any redirecting token does not meet that condition, the entire list of tokens is invalid.
 * 
 * @param command 
 * @return int 
 */
int valid(struct LinkedList * tokens) {

    struct DLLNode * current = tokens -> head;
    struct DLLNode * previous = NULL;

    while(current != NULL) {
        if( (strcmp(current -> value, ">") == 0 || strcmp(current -> value, "<") == 0 || strcmp(current -> value, "|") == 0)) {
            if(previous == NULL || current -> next == NULL) {
                return FALSE;
            }

            if(previous != NULL && (strcmp(previous -> value, "<") == 0 || strcmp(previous -> value, "<") == 0 || strcmp(previous -> value, "|") == 0)) {
                return FALSE;
            }
        }
        previous = current;
        current = current -> next;
    }
    return TRUE;
}

void separateTokens(char *array) {
    int i = 0;
    int j = 0;

    while (array[i] != '\0') {
        if (isspace((unsigned char)array[i])) {
            if (j > 0 && !isspace((unsigned char)array[j - 1])) {
                array[j] = ' ';
                j++;
            }
        } else {
            array[j] = array[i];
            j++;
        }
        i++;
    }

    if (j > 0 && !isspace((unsigned char)array[j - 1])) {
        array[j] = ' '; // Adding space at the end if needed
        j++;
    }
    
    array[j] = '\0'; // Null-terminate the modified string
}

/**
 * @brief Parses the given command-line into a struct command.
 * 
 * @param buffer the command-line, given as a buffer
 * @param size the number of bytes in buffer
 * @return struct command* 
 */
struct command * parse(char * buffer) {

    /**
     * @brief How do we do this, algorithimically?
     * 
     * We tokenize the buffer. For simplicity, we will induce spaces between each token prior to parsing.
     * 
     * We push each token of the buffer into a List.
     * 
     * Once we have pushed every token, we begin reading the list from the start.
     * 
     * The first element encountered will always (under proper circumstance, i.e., the user does not give an invalid command) be the program to execute.
     * 
     * Then, we read off the next token.
     * 
     * If the token is '<' then we know that the next token following this token will be the standard input for the program.
     * If the token is '>' then we know that the next token following this token will be the standard output for the program.
     * If the token is '|' then we know that the next token following this is indeed another program; the input of which is the output of the current program.
     */

    separateTokens(buffer);
    struct LinkedList * tokens = tokenize(buffer);
    if(TESTING) {
        traverseLL(tokens);
    }


    if(valid(tokens)) { //A valid command
        struct command * command = transform(tokens);
        free_DLL(tokens);
        return command;
    }
    else { //An invalid command
        free_DLL(tokens);
        return NULL;
    }

}