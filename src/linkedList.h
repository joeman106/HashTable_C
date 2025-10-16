#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>

/**
 * Node definition for linked list, each node contains a key/value pair and ptr to next node. The linked list will hold 
 * the key as a void pointer, it will also store the maximum allowed system variable size_t for the frequency, which will 
 * be a 64 bit integer on most modern systems, and a 32 bit on older ones. This choice is a bit overkill, but ensures that 
 * even the largest realistic datasets can be hashed without overflow, as overflow on a 64 bit system would require more 
 * than 64 bits of keys somehow hashing to the same value. The node also already contains 64 bits of information either way
 * VIA the next node pointer, so a 64 bit integer isn't all that insane considering that.
 * 
 * I opted to implement key ownership within the actual node, meaning that keys must be dynamically allocated
 * (eg via strdup). As such, the free_list function frees both the nodes themselves and the keys they reference.
 * 
*/

typedef struct Node
{
    void*        key;
    size_t       freq;
    struct Node* next;
} Node;

/* 
    Creates a new node, allocates necessary memory and initializes data. Next defaults to NULL
*/

Node* create_node(void* key, size_t freq);

/* 
    Inserts a node with a specified key (generic void pointer) and it's frequency. Implements push-to-front insertion, 
    since order doesn't matter in a hashtable, and it's O(1) complexity.
*/

void insert_node(Node** head, void* key, size_t freq);

/*
    Traverse the list and free all memory allocated from the heap within the node, dereferencing the head ptr 
    as well. I also opted to free the void pointer, which limits flexibility in that the hashtable will 
    now likely crash if it tries to free a key that has been added from the stack, but this seems like the most 
    logical approach.
*/

void free_list(Node** head);

#endif