#include <stdio.h>
#include <stdlib.h>

struct DLLNode {
    struct DLLNode * prev;
    struct DLLNode * next;
    char * value;
};

struct LinkedList {
    struct DLLNode * head;
    struct DLLNode * tail;
};

/**
 * Allocates and returns a  new DLL Node whose value contains the given value.
*/
struct DLLNode * newDLLNode(char * value) {
    struct DLLNode * node = (struct DLLNode *)malloc(sizeof(struct DLLNode));
    node -> value = value;
    node -> prev = NULL;
    node -> next = NULL;
    return node;
}

/**
 * Appends the given value to the front of the given linked list.
*/
void add_front(struct LinkedList * list, char * value) {
    struct DLLNode * node = newDLLNode(value);
    if(list -> head == NULL) {
        list -> head = node;
        list -> tail = node;
    }
    else {
        list -> head -> prev = node;
        node -> next = list -> head;
        list -> head = node;
    }
}

/**
 * Appends the given value to the end of the given linked list.
*/
void add_tail(struct LinkedList * list, char * value) {
    struct DLLNode * node = newDLLNode(value);
    if(list -> tail == NULL) {
        list -> head = node;
        list -> tail = node;
    }
    else {
        list -> tail -> next = node;
        node -> prev = list -> tail;
        list -> tail = node;
    }
}

void traverseLL(struct LinkedList * list) {
    struct DLLNode * node = list -> head;
    while(node != NULL) {
        printf("'%s' -> ", node -> value);
        node = node -> next;
    }
    printf("NULL\n");
}

struct LinkedList * newLinkedList() {
    struct LinkedList * list = (struct LinkedList *)malloc(sizeof(struct LinkedList));
    list -> head = NULL;
    list -> tail = NULL;
    return list;
}

char * remove_front(struct LinkedList * list) {
    if(list -> head != NULL) {
        struct DLLNode * node = list -> head;
        char * entry = node -> value;

        if(list -> head == list -> tail) {
            list -> tail = NULL;
        }

        list -> head = node -> next;
        if(node -> next != NULL) {
            node -> next -> prev = NULL;
        }
        free(node);
        return entry;
    }
    return NULL;
}
void free_DLL(struct LinkedList * list) {
    struct DLLNode * node = list -> head;
    while(node != NULL) {
        struct DLLNode * temp = node -> next;
        free(node -> value);
        free(node);
        node = temp;
    }
    free(list);
}