#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "stack.h"
#include "DLL.h"
#include "parse.h"



size_t size(char * buffer) {
    size_t i = 0;
    while(*buffer) {
        i++;
        buffer++;
    }
    return i;
}

void DLL_test() {
    struct LinkedList * list = newLinkedList();

    char * entry = malloc(2*sizeof(char));
    entry[0] = 'h';
    entry[1] = '\0';

    char * entry2 = malloc(2*sizeof(char));
    entry2[0] = 'w';
    entry2[1] = '\0';
    

    add_tail(list, entry2);

    add_front(list, entry);
    traverseLL(list);

    char * output = remove_front(list);
    printf("Output is : '%s'\n", output);



    traverseLL(list);

    output = remove_front(list);
    printf("Output is : '%s'\n", output);

    free(output);

    traverseLL(list);
    free_DLL(list);

}

void parseTest() {
    // char * buffer = "foo quux < bar > baz ";

    char * buffer = "foo | bar < baz ";
    struct command * command = parse(buffer);
    free_struct_command(command);
}
int main(void) {

    parseTest();
    return 0;
}