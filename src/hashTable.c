#include "hashTable.h"
#include "crc64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HashTable* create_table(HashFunction hash_func, size_t capacity, double max_load){
    HashTable* new_table = (HashTable*) malloc(sizeof(HashTable));
    if(!new_table){
        fprintf(stderr, "Error: HashTable structure memory allocation failed!");
        exit(EXIT_FAILURE);
    }

    new_table -> buckets = (Node**) malloc(sizeof(Node*) * capacity);
    if(!new_table -> buckets){
        fprintf(stderr, "Error: HashTable bucket memory allocation failed!");
        exit(EXIT_FAILURE);
    }
    //Initialize all buckets to be null. Opted against calloc because a null check (when inserting) is cleaner IMO.
    for(size_t i = 0; i < capacity; i++){
        new_table -> buckets[i] = NULL;
    }

    new_table -> capacity = capacity;
    new_table -> size = 0;
    new_table -> collisions = 0;
    new_table -> max_load = max_load;
    new_table -> hash_func = hash_func;

    return new_table;
}

void insert_node_data(HashTable** pht, void* key){
    HashTable* ht = *pht;
    size_t hash = hash_val(ht -> hash_func, key); //Find hash, idx in buckets of hashed value. Uses desired hash function
    size_t idx = INDEX(hash, ht -> capacity);

    //If nothing is at the index, just insert a new node with all data and jump to check max load.
    if(!ht -> buckets[idx]){
        int freq = 1;
        insert_node(&ht -> buckets[idx], key, freq);
    } else {
        Node* node_at_idx = ht -> buckets[idx];
        Node* curr = node_at_idx;
        //If there is a node there, traverse the list to see if the key is already in one of the nodes.
        while(curr != NULL){
            //If it is, increment the frequency and move on. No max load check needed since a node isn't being added.
            if(strcmp((char*) curr -> key, (char*) key) == 0){
                curr -> freq += 1;
                free(key);
                return;
            }
            curr = curr -> next;
        }
        /*
            If no node on the list has the same key, create a new frequency variable that is 1 (since this is the first occurance),
            then create a new node and make it's next ptr point to the correct index of the array. Then I simply store it in the
            array. Increment collisions.
        */
        size_t freq = 1;
        node_at_idx = create_node(key, freq);
        node_at_idx -> next = ht -> buckets[idx];
        ht -> buckets[idx] = node_at_idx;
        ht -> collisions++;
    }

    //Increment the size since we only get here if a node has been added, then check to see if the maximum load has been reached.
    ht -> size++;
    double load = (double) ht -> size / (double) ht -> capacity;
    if(load > ht -> max_load) grow_table(pht, GROWTH_FACTOR);
}

void grow_table(HashTable** ht, int growth_factor){
    HashTable* old_table = *ht;
    HashFunction hash_func = old_table -> hash_func;
    size_t new_capacity = old_table -> capacity * (size_t) growth_factor;
    HashTable* new_table = create_table(hash_func, new_capacity, old_table -> max_load);

    for (size_t i = 0; i < old_table->capacity; ++i) {
        Node* cur = old_table->buckets[i];
        while (cur) {
            Node* next = cur->next;

            size_t hash = hash_val(hash_func, cur->key);
            size_t idx = INDEX(hash, new_table->capacity);

            cur->next = new_table->buckets[idx];
            new_table->buckets[idx] = cur;

            cur = next;
        }
    }

    /*
        Traverse all nodes in the new table and increment collisions accordingly. The new table will most likely have a
        different amount of collisions than the old table, but the size will remain the same since the same amount of 
        nodes have been inserted, so the new table can just carry over the old size.
    */

    new_table -> size = old_table -> size;
    new_table -> collisions = 0;
    for(size_t i = 0; i < new_table -> capacity; ++i){
        Node* curr = new_table -> buckets[i];
        if (curr && curr -> next) {
            while(curr -> next){
                new_table -> collisions++;
                curr = curr -> next;
            }
        } 
    }

    free(old_table -> buckets);
    free(old_table);

    *ht = new_table;
}

size_t hash_val(HashFunction hash_func, void* val){
    return (size_t) hash_func((char*) val);
}

void free_table(HashTable* ht){
    for(size_t i = 0; i < ht -> capacity; i++){
        free_list(&ht -> buckets[i]);
    }
    free(ht -> buckets);
    free(ht);
}