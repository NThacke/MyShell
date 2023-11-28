struct DLLNode {
    struct DLLNode * prev;
    struct DLLNode * next;
    char * value;
};

struct LinkedList {
    struct DLLNode * head;
    struct DLLNode * tail;
};

struct LinkedList * newLinkedList();

void add_front(struct LinkedList * list, char * value);

void add_tail(struct LinkedList * list, char * value);

void traverseLL(struct LinkedList * list);

char * remove_front(struct LinkedList * list);

void free_DLL(struct LinkedList * list);
