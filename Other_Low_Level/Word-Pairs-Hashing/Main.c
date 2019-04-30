#include "getWord.h"
#include "sTools.h"
#include "hashingTools.h"
#include <stdio.h>
#include <limits.h>
#include <assert.h>

#define Pairs struct pairs


/*
Name: Tam Nguyen
Assignment 1: Word Pairs
Description: a program (in C) targeted at the Linux platform which reads words from one or more files, 
    and prints out a list of the most frequently occurring sequential pairs of words and the number of times they occurred, 
    in decreasing order of occurrence. A hash table feature is required for this assignment.
Class: CS360
Professor: Ben McCammish
*/



void parseArguments(char *argv);
Pairs **readFile(char fileLocation[], Pairs **hashTable);
void convertTo1DArray(Pairs ** hashTable, Pairs newTable[], unsigned long long tableSize, unsigned long long totalElement);
int comparator(const void* a, const void* b);
void printArrTable(Pairs table[], unsigned long long tableSize);

unsigned long long htSize = 100, elementsCount = 0;
int wordCount = -1;

int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "To run, do: <program> -<count> <filename> <file...>\n\n");
        return -1; // argv[0] is the program name
    }
    parseArguments(argv[1]);

    Pairs **hashTable = initTable((int)htSize);
    if (!hashTable) {
        fprintf(stderr, "Error making table\n");
        return hashTable;
    }
    for (int i = (wordCount > 0)? 2:1; i < argc; i++) { // see if user gave us a wordCount
            hashTable = readFile(argv[i], hashTable);
    }

    Pairs nTable[elementsCount];
    convertTo1DArray(hashTable, nTable,htSize,elementsCount);
    qsort(nTable, elementsCount, sizeof(Pairs), comparator);
    printArrTable(nTable,elementsCount);
    freeHTable(hashTable,htSize);
    free(hashTable);
}

void parseArguments(char *argv) { // get count to display and files to read
    wordCount = (int)atoi(argv); // try convert arg to int
    if (wordCount >= 0) { // if the user didnt give us the corrent '-#' arg then we want to print all.
        wordCount = -1; // -1 indicate to print all pairs
    } else {
        wordCount = abs(wordCount); // since the argument is -#, we want to make sure it's convert as positive number
    }
}

Pairs **readFile(char fileLocation[], Pairs **hashTable) { // Open a file and pass it to getNextWord.
    FILE *file = fopen(fileLocation, "r");

    if(file == NULL) {
        fprintf(stderr, "Error opening %s\n",fileLocation);
        return hashTable;
    }

    int count = 1;
    char *s;
    char *w1; // first word of pairs
    while ((s=getNextWord(file)) != NULL) {
        if (count == 1) { // this is only gonna get ran once
            w1 = concat(s,"");
            count++;
        } else {
            char *w2 = concat(w1, s); // merge it to get our pairs
            if (w2 != NULL || strcmp(w2, "") != 0) {
                Pairs *node = searchFor(w2, hashTable, htSize);
                free(w1); // dont need this anymore since we paired it
                if (node != NULL) { // the word is already in the hash table
                    node->occur++;
                }
                else {
                    double loadFactor = (elementsCount/(htSize/1.0));
                    if (loadFactor >= 0.8) {// our load factor for expanding
                        Pairs **tmpTable = growTable(hashTable, &htSize);
                        if (tmpTable == NULL) {
                            fprintf(stderr, "Fail to expand table! Keeping table size of %llu\n", htSize);
                        } else {
                            safelyFree(NULL, hashTable);
                            hashTable = tmpTable; // don't need the old table
                        }
                    }
                    if (insert(w2, hashTable, htSize) == 1) {// if successfully inserted
                        elementsCount++;
                    }
                }
            }       
            free(w2);
            w1 = concat(s,""); // this is the start of next pair
        }
        free(s);
    }
    free(w1);
    fclose(file);
    return hashTable;
}


void convertTo1DArray(Pairs ** hashTable, Pairs newTable[], unsigned long long tableSize, unsigned long long totalElement) {
    for (unsigned long long i = 0,n = 0; i < tableSize && n < totalElement; i++) {
        Pairs *tmp = hashTable[i];
        while (tmp) {
            newTable[n].s = tmp->s;
            newTable[n].occur = tmp->occur;
            newTable[n].hash = 0; // dont care bout these
            newTable[n].next = NULL;         
            n++;
            tmp = tmp->next;
        }
    }
}


// print the array table
void printArrTable(Pairs table[], unsigned long long tableSize) {
    for (int i = 0; i < tableSize; i++)
    {
        if (wordCount > 0) {
            fprintf(stdout," %10d %s\n", table[i].occur, table[i].s);
            wordCount--;
        }
        else if ( wordCount < 0){
            fprintf(stdout," %10d %s\n", table[i].occur, table[i].s);
        }
        else // =0
            return;
    }
}


// sort in descending order
int comparator(const void* p, const void* q){
    Pairs *vp,*vq;
    vp = (Pairs *)p;
    vq = (Pairs *)q;
    return vq->occur-vp->occur;
}
