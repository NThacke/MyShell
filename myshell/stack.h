struct stack_node {
    char * entry;
    struct stack_node * next;
};

struct stack {
    struct stack_node * element;
};

struct stack * new_stack();

void free_stack(struct stack * stack);

void push(struct stack * stack, char * entry);

char * pop(struct stack * stack);

void traverse_stack(struct stack * stack);