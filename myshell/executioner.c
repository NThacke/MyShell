#include <stdio.h>
#include <stdbool.h>
#include "parser.h"
#include <string.h>

bool built_in_detector(char *test_string) {
    if (strcmp(test_string, "pwd") == 0) {
        return true;
    }
    if (strcmp(test_string, "which") == 0) {
        return true;
    }
    if (strcmp(test_string, "cd") == 0) {
        return true;
    }
    return false;
}

/* grab command test
 *
 */
void execute_command(struct command * c) {
    // get command
    printf("command: %s\n", c->files[0]->name);
    fflush(stdout);
    for (int x = 1; x < c->files[0]->size; x++) {
        printf("arg: %s",c->files[0]->args[x]);
        fflush(stdout);
    }

    if ( built_in_detector(c->files[0]->name) == true) {
        printf("BUILT IN DETECTED\n");
        fflush(stdout);
        return;
    }

    // output
    FILE *pf;
    char data[5120];

    // Setup our pipe for reading and execute our command.
    pf = popen(c->files[0]->name,"r");

    if(!pf){
        fprintf(stderr, "Could not open pipe for output.\n");
        return;
    }

    // Grab data from process execution
    while (fgets(data, sizeof(data), pf) != NULL)
    {
        printf("Received: '%s'\n", data);
    }

    // Print grabbed data to the screen.
    printf("%s",data);
    fflush(stdout);

    if (pclose(pf) != 0)
        fprintf(stderr," Error: Failed to close command stream \n");

}