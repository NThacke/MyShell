#include <stdlib.h>

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

void free_struct_command(struct command * c);

struct command * parse(char * current_dir, char * buffer, size_t size);

void traverse_command(struct command * command);