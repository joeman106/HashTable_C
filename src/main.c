#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "hashTable.h"
#include "getWord.h"
#include "crc64.h"

#define MINIMUM_ARGS 2 //Minimum arguments needed for file to run. The executable file, and at least one read file.
#define INITIAL_CAPACITY 64 //Initial capacity of the Hash Table

/*
    If the load (size/capacity) exceeds this, the table will regrow. This was the most optimal max load for speed with an average process 
    time of ~0.3 sec, even dipping to 0.29 on one run. I tested numbers here from 0.5 - 3 and most ran closer to 0.31 - 0.32 seconds, with 
    a 7.4mb dataset and fresh output file before each run, as well as the exact same initial capacity.
*/

#define MAX_LOAD 1

/*
    Improves readability so it's more clear why I add two bytes when reallocating memory for word pair string.
*/

#define NULL_TERM_LEN 1
#define SPACE_LEN 1

//Definition of a struct for easier storage of word pairs with their frequencies.
typedef struct _PairCount{ 
    char* key;
    size_t freq;
} PairCount;

//Custom compare function for qsort to sort in ascending order. Sorts alphabetically if they are tied.
int compare(const void* x, const void* y){
    const PairCount *pair_one = x;
    const PairCount *pair_two = y;

    if(pair_one -> freq > pair_two -> freq) return -1;
    if(pair_one -> freq < pair_two -> freq) return 1;
    
    return strcmp(pair_one -> key, pair_two -> key);
}

int main(int argc, char* argv[]){

    clock_t begin = clock();

    if(argc < MINIMUM_ARGS){
        fprintf(stderr, "Error: Wrong amount of arguments!\nUsage: pairsofwords <-count> fileName1 <fileName2> <fileName3>...\n");
        exit(EXIT_FAILURE);
    }

    size_t display_count = 0;
    size_t non_file_args = 1; //The first argument has to not be a file no matter what.
    //Convert argument 1 into a digit if it starts with '-', make sure it is a base-10 digit otherwise throw an error.
    if (argv[1][0] == '-') {
        if (argv[1][1] == '\0') {
            fprintf(stderr, "Error: expected -<count> but got \"-\"\n");
            exit(EXIT_FAILURE);
        }

        char *endptr = NULL;
        display_count = strtoul(argv[1] + 1, &endptr, 10);
        if (*endptr != '\0') {
            fprintf(stderr, "Error: -count must be a base-10 integer, not \"%s\"\n", argv[1] + 1);
            exit(EXIT_FAILURE);
        }

        non_file_args++;
    }

    HashTable* ht = create_table(crc64, INITIAL_CAPACITY, MAX_LOAD);

    //Open the file
    for(size_t i = non_file_args; i < (size_t) argc; i++){
        FILE* fp = fopen(argv[i], "r");
        if(!fp){
            fprintf(stderr, "Error: file %s was not found!\n", argv[i]);
            exit(EXIT_FAILURE);
        }

        char* prev_word = getNextWord(fp);

        while(prev_word){
            //Create the word pair string using snprintf to print to the string buffer, easiest way to add the space in between.
            char* curr_word = getNextWord(fp);
            if(!curr_word) break;

            size_t word_pair_length = strlen(prev_word) + SPACE_LEN + strlen(curr_word) + NULL_TERM_LEN;
            char* word_pair = malloc(word_pair_length);
            if(!word_pair){
                fprintf(stderr, "Error: Word pair string allocation failed!\n");
                exit(EXIT_FAILURE);
            }

            snprintf(word_pair, word_pair_length, "%s %s", prev_word, curr_word);
            insert_node_data(&ht, word_pair);
            free(prev_word);
            prev_word = curr_word;
        }
        free(prev_word); //Free the last word.
        fclose(fp);
    }

    //Now that everything has been placed in the hashtable and frequencies counted, I copy all word pairs to an array and sort it.
    PairCount* pairs_with_counts = malloc(sizeof(PairCount) * ht -> size);
    if(!pairs_with_counts){
        fprintf(stderr, "Error: Word pair array memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    int pairs = 0;
    for (size_t i = 0; i < ht->capacity; i++) {
        for (Node* curr = ht->buckets[i]; curr; curr = curr->next) {
            pairs_with_counts[pairs].key  = curr->key;
            pairs_with_counts[pairs].freq = curr->freq;
            pairs++;
        }
    }


    qsort(pairs_with_counts, pairs, sizeof(*pairs_with_counts), compare);


    size_t pairs_to_print = display_count > 0 ? display_count : (size_t) pairs;


    FILE *fOut = fopen("output.txt", "w");
    if(!fOut){
        fprintf(stderr, "Error: Could not open output file!\n");
        exit(EXIT_FAILURE);
    }

    for(size_t i = 0; i < pairs_to_print; i++){
        char* pair = pairs_with_counts[i].key;
        int occ = (int) pairs_with_counts[i].freq;
        fprintf(fOut, "%10d %s\n", occ, pair);
    }

    fclose(fOut);
    free(pairs_with_counts);
    printf("Process finished, final table had %zu collisions\n", ht -> collisions);
    free_table(ht);

    clock_t end = clock();
    double process_time = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Process completed in %f seconds", process_time);

    return 0;
}