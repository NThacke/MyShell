#include <stdio.h>
#include <stdlib.h>
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

    struct command * command = parse(buffer, length);
    // free_struct_command(command);

    traverse_command(command);
}
int main(void) {

    test0();
}