#C HashTable implementation

A generic, lightweight HashTable that can accept any string as a key. It accepts any hashing function that hashes strings as char arrays.
Collisions are resolved using a custom linked list implementation. An example program is included in this repo (main.c) that demonstrates 
the hash table's capabilities, it counts all word pairs in any given .txt file and returns an output file with all the word pairs printed 
next to the number of occurances of the word pair, sorted via qsort from most frequent to least frequent. When running the program, an 
optional argument can be passed that lets you choose how many of the word pairs to print in the output file. For example, a count of 5 
would only print the 5 most common words with their frequencies. If no count argument is passed, the program will default to printing 
every word pair and frequency. Multiple .txt files can be passed as arguments at the same time, the program will iterate over all of them 
and print to the output file at the end.

The crc64 hash is included in the helperfunctions file. To use a different hashing function, the makeFile must be modified accordingly.


To compile the program, use make run ARGS= <count> file1.txt <file2.txt> <file3.txt> ...
