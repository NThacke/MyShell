#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

/**
 * @brief Determines whether the given input is a valid which command. If so, this function returns 0 (true), otherwise, it returns non-zero (false).
 * 
 * @param input 
 * @return int 
 */
int valid_which(char * input) {
    return input[0] == 'w' && input[1] == 'h' && input[2] == 'i' && input[3] == 'c' && input[4] == 'h' && input[5] == ' ';
}
/**
 * @brief Determines and returns the program which the command 'which' is referring to.
 * 
 * @param input Guaranteed to be atleast 5120 bytes long, as determiend by main()
 * @return char* malloced on the heap; must be freed
 */
char * which_program_name(char * input) {
    if(!(input[0] == 'w' && input[1] == 'h' && input[2] == 'i' && input[3] == 'c' && input[4] == 'h' && input[5] == ' ')) {
        return NULL; //invalid, not a which command!
    }
    char * name = malloc((5120-6)*sizeof(char));
    strcpy(name, (input+6)); //copy the name, which starts at index 6

    if(strlen(name) > 0) {
        return name;
    }
    else {
        free(name);
        return NULL;
    }
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
 * @brief Searches for the given program amongst the directories "/usr/local/bin", "/usr/bin", and "/bin", in that order. If the program name does not exist in any of those directories, this function will return NULL.
 * 
 * @param program 
 * @return char* 
 */
char * program_path(char * program) {

    char * path = NULL;

    if(path == NULL) {
        path = search_directory(program, "/usr/local/bin");
    }
    if(path == NULL) {
        path = search_directory(program, "/usr/bin");
    }
    if(path == NULL) {
        path = search_directory(program, "/bin");
    }

    if(path != NULL) {
        char * path_and_program = malloc((strlen(program) + strlen(path)+2) * sizeof(char));
        int index = 0;
        while(*path) {
            path_and_program[index] = *path;
            path++;
            index++;
        }

        path_and_program[index] = '/';
        index++;

        while(*program) {
            path_and_program[index] = *program;
            program++;
            index++;
        }

        return path_and_program;
    }
    return path;

}
/**
 * @brief Determines and prints the absolute path to a built-in program name (of the likes of cat, ls, rm; i.e, UNIX standard programs). If the given program is not found, an error message displays as such. If the given command is invalid, this program displays as such.
 * 
 * @param input 
 */
void which(char * input) {
    // Stub
    //We have guaranteed outselves that the beginning of input is indeed 'which '.
    //Now we must find the program that follows it.
    char * program_name = which_program_name(input);
    if(program_name != NULL) {
        // printf("Program name is '%s'\n", program_name);

        //Now we look for the program name.
        char * path = program_path(program_name);
        if(path == NULL) {
            printf("'%s' was not found.\n", program_name);
        }
        else {
            printf("%s\n", path);
            free(path);
        }
        free(program_name);
    }
    else {
        printf("Invalid program name given to 'which' command.\n");
    }
}