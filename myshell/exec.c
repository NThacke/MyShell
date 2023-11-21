#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include <string.h>


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
    return strcmp(name, "which") || strcmp(name, "pwd") || strcmp(name, "cd");
}
/**
 * @brief Executes the given file, which must be a built-in command.
 * 
 * @param file 
 */
void execute_built_in(struct file * file) {
    if(built_in(file)) { //robust checking
        
    }
}
void execute(struct command * command) {
    
    for(int i = 0; i<command -> size; i++) {
        
        struct file * current_file = command -> files[i];

        if(built_in(current_file)) {
            execute_built_in(current_file);
        }
        else {

        }
    }


}