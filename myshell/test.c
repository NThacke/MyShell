#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "parser.h"



size_t size(char * buffer) {
    size_t i = 0;
    while(*buffer) {
        i++;
        buffer++;
    }
    return i;
}
void test0() {
    char * buffer = "foo bar > quux | baz bar bis ness";
    size_t length = size(buffer);
    
    char cwd [PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    struct command * command = parse(cwd, buffer, length);
    // free_struct_command(command);

    traverse_command(command);
}

void test1() {
    char * buffer = "foo | bar > baz";
    size_t length = size(buffer);
    char cwd [PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    struct command * command = parse(cwd, buffer, length);
    // free_struct_command(command);

    traverse_command(command);
}
/**
 * @brief Tests overriding pipe functionality.
 * 
 * "foo | bar < baz" 
 * 
 * Denotes that foo pipes into bar, but that bar has its input from baz. 
 */
void test_override() {
    char * buffer = "foo | bar < baz";
    size_t length = size(buffer);
    char cwd [PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    struct command * command = parse(cwd, buffer, length); //no need to free.. (causes double free)... why?
}
int main(void) {

    test_override();
}