#include "hashingTools.h"
#include <assert.h>

#define BigInt unsigned long long
int growFactor = 3;

/*Note to me:
 when you see a statement if(ptr) it just mean its checking if node is not empty
*/
/*Not to prof:
some statement I wont add a comment because the print statement bellow it tell you what it is
*/

// print hash table
void printTable(Pairs **htable, BigInt htSize, int printCollision) {
    for (BigInt i = 0; i < htSize; i++) {
        printf("HT[%llu]: ", i);
        Pairs *tmp = htable[i];
        while (tmp != NULL) { // print the collisions
            printf("|%s|@%llu w/ %d occurance -> ", tmp->s, tmp->hash%htSize, tmp->occur);
            if (!printCollision)    break;
            tmp = tmp->next;
        }
        printf("\n");
    }
}

// function to get us free space for our hash table
Pairs **initTable (int tSize) {
    Pairs **tmp = malloc(tSize *sizeof(Pairs*));
    if (!tmp) // if it's null call this function untill we get our space
        tmp = initTable(tSize);
    return tmp;
}

// function to insert our word pairs into the hash table
int insert(char *s, Pairs **hashTable, BigInt tableSize) {
    if (!s) {
        fprintf(stderr, "Inserting NULL string? Why?\n");
        return 0;
    }
    assert(hashTable != NULL);
    BigInt hash = crc64(s);
    BigInt indx = hash%tableSize;
    Pairs *tmp = hashTable[indx];
    Pairs *toAddTo;

    int indexEmpty = 1;
    while (tmp != NULL) { 
        indexEmpty = 0; // check to to see if this is the first element in indx
        toAddTo = tmp;
        tmp = tmp->next;
    }

    Pairs *node = (Pairs*)malloc(sizeof(Pairs));
    node->s = strdup(s);
    node->occur = 1;
    node->hash = hash;
    node->next = NULL;

    if (indexEmpty) {
        hashTable[indx] = node;
    } else {
        toAddTo ->next = node;
    }
    return 1;
}

// only searching and return us the node if it occurs
Pairs *searchFor (char *s, Pairs **hashTable, BigInt tableSize) {
    BigInt indx = crc64(s)%tableSize;
    Pairs *node = hashTable[indx];
    while (node) { // check collision
        if (strcmp(node->s, s) == 0)// matches
            return node;
        node = node->next;
    }
    return NULL; // // only care about the node we're searching for
}


// expanding hash table
Pairs **growTable(Pairs **hashTable, BigInt *tableSize) {
    assert(hashTable != NULL);
    Pairs **newTable = initTable(*tableSize * growFactor); // grow factor is global var (line 5)
    if (!newTable) { // FAILED to make new table
        return NULL;
    }
    while (checkTableNULL(newTable,*tableSize * growFactor)) { // make sure the new table is empty
        fprintf(stderr, "Failed to expand table, retrying...\n");
        free(newTable);
        newTable = initTable(*tableSize * growFactor);        
    } 
    //printf("Rehashing table..\n");
    rehashElements(hashTable, newTable, *tableSize);
    *tableSize *= growFactor; // rehashed successful, so change the real table size tracker
    return newTable;
}

// rehash the elements to the new table
void rehashElements(Pairs **hashTable, Pairs **newTable, BigInt tableSize) {
    BigInt newTableSize = growFactor * tableSize;
    int n = 0; // keep track of how many elements rehashed (for debugging)
    assert(checkTableNULL(newTable,newTableSize) == 0); // making sure it's empty

    for (BigInt i = 0; i < tableSize; i++) { // traverse through old table
        if (hashTable[i]) { // if there is an element in the index
            Pairs *tmp = hashTable[i];
            Pairs *toBeRehashed;
            do { // traverse through the linked list in that index and rehash the elements
                n++;
                toBeRehashed = tmp;
                tmp = tmp->next; // go to the next node in LL
                BigInt newIndx = toBeRehashed->hash % newTableSize;
                Pairs *collisionDetect = newTable[newIndx]; // check if a new already occupies it
                if (collisionDetect) {
                    while(collisionDetect->next) {
                        collisionDetect = collisionDetect->next;
                    }
                    collisionDetect->next = toBeRehashed;
                }
                else
                    newTable[newIndx] = toBeRehashed; // set them to the new location
                toBeRehashed->next = NULL; // since we assigned it to new spot, make sure it's not pointing to anything
            } while (tmp != NULL);
            hashTable[i] = NULL; // make the old one points to nothing
        }
    }
    //printf("rehashed %d elements\n",n);
}

// free hash table
void freeHTable(Pairs **hashTable, BigInt tableSize) {
    for (BigInt i = 0; i < tableSize; i++) {
        hashTable[i] = freeIndx(hashTable[i]);
    }
    //printf("\nAll Freed\n");
}

// free any elements and collision in a index
// basic link list traversal
Pairs *freeIndx(Pairs *ptrToIndx) {
    Pairs *toBeDeleted;
    while (ptrToIndx) { // get the collisions
        toBeDeleted = ptrToIndx;
        ptrToIndx = ptrToIndx->next;
        toBeDeleted->next = NULL;
        free(toBeDeleted->s); // since we use strdup to store it
        safelyFree(toBeDeleted, NULL);
    }
    return NULL;

}

//making sure we're safely freeing (prevent double free)
void safelyFree(Pairs *node, Pairs **hTable) { // prevent double free
    if (!node)
    {
        if (hTable) {
            free(hTable);
            hTable = NULL;
        }

        return;
    }
    free(node);
    node = NULL;
    if (hTable) {
        free(hTable);
        hTable = NULL;
    }
    return;
}

// check to see if table is pointing to null for each element
int checkTableNULL(Pairs **hashTable, BigInt tableSize) {
    for (int i = 0; i < tableSize; i++) {
        if (!hashTable[i])
            return i;
    }
    return 0;
}