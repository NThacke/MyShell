#include <stdio.h>
#include <stdlib.h>


struct stack_node {
    char * entry;
    struct stack_node * next;
};

struct stack {
    struct stack_node * element;
};

/**
 * @brief Creates and returns a new stack with empty elements.
 * 
 * @return struct stack* 
 */
struct stack * new_stack() {
    struct stack * stack = malloc(sizeof(struct stack));
    stack -> element = NULL;
    return stack;
}

struct stack_node * new_stack_node() {
    struct stack_node * node = malloc(sizeof(struct stack_node));
    node -> entry = NULL;
    node -> next = NULL;
    return node;
}

/**
 * @brief Frees the given stack from the heap; any further invocation of any of its elements will result in a SEGMENTATION FAULT.
 * 
 * @param stack 
 */
void free_stack(struct stack * stack) {
    struct stack_node * current = stack -> element;
    while(current != NULL) {
        if(current -> entry != NULL) {
            free(current -> entry);
        }
        struct stack_node * temp = current -> next;
        free(current);
        current = temp;
    }
    free(stack);
}

/**
 * @brief Pushes the given entry onto the top of the given stack. Note that the given entry MUST be malloced on the heap.
 * 
 * @param entry 
 */
void push(struct stack * stack, char * entry) {
    struct stack_node * node = new_stack_node();
    node -> entry = entry;
    node -> next = stack -> element;
    stack -> element = node;

}

/**
 * @brief Pops and returns the top of the stack. Note that you must free the returned char *
 * 
 * @param stack 
 * @return char* 
 */
char * pop(struct stack * stack) {
    struct stack_node * node = stack -> element;
    char * entry = node -> entry;
    stack -> element = node -> next;
    free(node);
    return entry;
}

void traverse_stack(struct stack * stack) {
    struct stack_node * current = stack -> element;
    while(current != NULL) {
        printf("'%s'\n", current -> entry);
        current = current -> next;
    }
}