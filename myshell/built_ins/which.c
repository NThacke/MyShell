#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>


#define TRUE 1
#define FALSE 0


/**
 * Returns true (1) if the given filename is a built-in command, such as "cd", "pwd", or "which"
*/
int built_in(char * filename) {
    return strcmp(filename, "cd") == 0 || strcmp(filename, "pwd") == 0 || strcmp(filename, "which") == 0;
}
/**
 * @brief Searches in the given directory for the given program. If the program exists in the directory, then this function returns the path to the program (absolute). Otherwise, this function will return NULL.
 * 
 * @param program 
 * @param directory 
 * @return char* 
 */
char * search_directory(char * program, char * directory) {
    struct dirent * entry = NULL;
    DIR * dir = opendir(directory);
    if(dir != NULL) {
        while( (entry = readdir(dir)) != NULL) {
            if(strcmp(entry -> d_name, program) == 0) {
                return directory;
            }
        }
        closedir(dir);
    }
    return NULL;
}
/**
 * Searches for and returns the absolute path for the given filename.
 * 
 * This method returns NULL when the filename does not exist amongst one of the following directories.
 * /usr/local/bin
 * /usr/bin
 * /bin
*/
char * find_path(char * filename) {
    char * path = NULL;

    if(!built_in(filename)) {
        if(path == NULL) {
            path = search_directory(filename, "/usr/local/bin");
        }
        if(path == NULL) {
            path = search_directory(filename, "/usr/bin");
        }
        if(path == NULL) {
            path = search_directory(filename, "/bin");
        }

        if(path != NULL) {
            char * path_and_program = malloc((strlen(filename) + strlen(path)+2) * sizeof(char));
            int index = 0;
            while(*path) {
                path_and_program[index] = *path;
                path++;
                index++;
            }

            path_and_program[index] = '/';
            index++;

            while(*filename) {
                path_and_program[index] = *filename;
                filename++;
                index++;
            }

            return path_and_program;
        }
    }
    return path;
}
/**
 * This program is the implementation of the unix command /which/.
 * 
 * /which/ takes a variable number of arguments, and will print the directory for the argument if the argument is a valid program (located under one of the following directories) :
 * 
 * /usr/local/bin, 
 * /usr/bin,
 * /bin, 
 * 
 * in that order.
 * 
 * 
*/
int main(int argc, char ** argv) {
    for(int i = 1; i<argc; i++) {
        char * path = find_path(argv[i]);
        if(path != NULL) {
            printf("%s\n", path);
            free(path);
        }
    }
}