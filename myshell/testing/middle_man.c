#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
    //Read from STDIN
    char c;
    while(read(STDIN_FILENO, &c, 1) > 0) {
        printf("%c", c);
    }
    printf("\n");
}