#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>

Node* create_node(void* key, size_t freq){
    Node* newNode = (Node*) malloc(sizeof(Node));

    if(!newNode){
        fprintf(stderr, "Error: Memory allocation for linked list failed!\n");
        exit(EXIT_FAILURE);
    }

    newNode -> key = key;
    newNode -> freq = freq;
    newNode -> next = NULL;

    return newNode;
}

/* Implements push-to-front insertion, since order doesn't matter in a hashtable, and it's O(1) complexity */
void insert_node(Node** head, void* key, size_t freq){
    Node* newNode = create_node(key, freq);
    newNode -> next = *head;
    *head = newNode;
}

/*
Traverse the list and free all memory allocated from the heap within the node, dereferencing the head ptr 
as well. I also opted to free the void pointer, which limits flexibility in that the hashtable will 
now likely crash if it tries to free a key that has been added from the stack, but this seems like the most 
logical approach.
*/

void free_list(Node** head){
    Node* cur = *head;
    while(cur){
        Node *next = cur -> next;
        free(cur -> key);
        free(cur);
        cur = next;
    }

    *head = NULL;
}