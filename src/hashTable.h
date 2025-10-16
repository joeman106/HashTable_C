#ifndef HASHTABLE_H
#define HASHTABLE_H

#define GROWTH_FACTOR 3
#define INDEX(hash, capacity) ((hash) % (capacity))

#include <stdint.h>
#include "linkedList.h"

/*
    Definition of a function pointer that is passed as an argument by the user to hash any given value.
    Accepts a void pointer which increases versatility.
*/

typedef unsigned long long (*HashFunction) (void* val);

/*
    This is my definition of the hash table structure. The struct will keep track of it's size, capacity, collisions,
    and buckets. It will be created with a max load that will trigger the grow_table function if reached/exceeded,
    and the max load will be a float between 0 and 1. It also accepts a function pointer to the desired hash function.

    Size, capacity, and collisions are also kept track of with the maximum system variable, size_t. This serves to
    be more consistent with the nodes the table holds as buckets, as their frequency tracking also utilizes the
    maximum system variable, as well as increasing the datasets the table can store. It will have a maximum capacity/size
    of 64 bits on a 64 bit system, which would make it functionally useful for stuff like small databases.
*/

typedef struct _HashTable{
    Node** buckets;
    size_t capacity;
    size_t size;
    size_t collisions;
    float  max_load; //Load = size/capacity
    HashFunction hash_func;
} HashTable;

/*
    Create a new table, allocating memory for the table itself plus the
    array of buckets, with the amount of allocated memory being equal to
    the size of a node structure times the specified initial size. Size/capacity
    variables will start at 0, and the max load will be specified in the top macro
    and put in the table.

    The buckets array will be a double pointer, because I will be changing the buckets array
    reference when I grow the table, as growing the table will require creating a new
    one with more buckets.
 */

HashTable* create_table(HashFunction hash_func, size_t size, double max_load);

/*
    Insert a given value into the table. This method will first check if a node exists in the index indicated by the hash.
    If not, it simply puts one there and sees if the maximum load has been reached. Otherwise, it iterates through the list
    to see if there is a node with the same key already in the list at the index. If so, it just increments frequency and
    returns without a load check. If not, it puts a new node in the correct index with a frequency of 1.

    It accepts the double pointer to the hashtable to make it easier to call grow_table, which accepts a double pointer.
    A single pointer reference ht is thus immediately made to do the operations on, and the double pointer pht is used to
    call grow_table.
*/

void insert_node_data(HashTable** pht, void* key);

/*
    Grow the table by a specified growth factor, using a double pointer reference to the desired table. A new table is created
    and all nodes from the old table are copied into it VIA rehashing. Size is copied over and collisions are recalculated after
    rehashing, and then the memory from the old tree is freed.
*/

void grow_table(HashTable** ht, int growth_factor);

/*
    Hash the given value, casting to a character array (string) prior to hashing.
*/

size_t hash_val(HashFunction hash_func, void* val);

/*
    Free all memory associated with the table, including the nodes, array for the buckets, and the structures themselves.
    This is accomplished by looping through each bucket and calling the free_list function from my linked list implementation,
    then simply freeing the array and table structure and dereferencing all pointers.
*/

void free_table(HashTable* ht);

#endif