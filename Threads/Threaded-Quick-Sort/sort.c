#include "sort.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <err.h>
#include <errno.h>

// CS 360 A8
// Task: Implement thread features where quick sort utilizes multiple task to speed up the process

#define SORT_THRESHOLD      40

typedef struct _sortParams {
    char** array;
    int left;
    int right;
} SortParams;

static void *threadedQS(void *arg);


static int maximumThreads = 1;              /* maximum # of threads to be used */
static int threadCount = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; 

/* This is an implementation of insert sort, which although it is */
/* n-squared, is faster at sorting short lists than quick sort,   */
/* due to its lack of recursive procedure call overhead.          */

static void insertSort(char** array, int left, int right) {
    int i, j;
    for (i = left + 1; i <= right; i++) {
        char* pivot = array[i];
        j = i - 1;
        while (j >= left && (strcmp(array[j],pivot) > 0)) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = pivot;
    }
}

/* Recursive quick sort, but with a provision to use */
/* insert sort when the range gets small.            */

static void quickSort(void* p) {
    SortParams* params = (SortParams*) p;
    char** array = params->array;
    int left = params->left;
    int right = params->right;
    int i = left, j = right;
	pthread_t task = 0, task2 = 0; // our thread
    if (j - i > SORT_THRESHOLD) {           /* if the sort range is substantial, use quick sort */

        int m = (i + j) >> 1;               /* pick pivot as median of         */
        char* temp, *pivot;                 /* first, last and middle elements */
        if (strcmp(array[i],array[m]) > 0) {
            temp = array[i]; array[i] = array[m]; array[m] = temp;
        }
        if (strcmp(array[m],array[j]) > 0) {
            temp = array[m]; array[m] = array[j]; array[j] = temp;
            if (strcmp(array[i],array[m]) > 0) {
                temp = array[i]; array[i] = array[m]; array[m] = temp;
            }
        }
        pivot = array[m];

        for (;;) {
            while (strcmp(array[i],pivot) < 0) i++; /* move i down to first element greater than or equal to pivot */
            while (strcmp(array[j],pivot) > 0) j--; /* move j up to first element less than or equal to pivot      */
            if (i < j) {
                char* temp = array[i];      /* if i and j have not passed each other */
                array[i++] = array[j];      /* swap their respective elements and    */
                array[j--] = temp;          /* advance both i and j                  */
            } else if (i == j) {
                i++; j--;
            } else break;                   /* if i > j, this partitioning is done  */
        }
		
        SortParams first;  first.array = array; first.left = left; first.right = j;
        if (maximumThreads > 0 && maximumThreads > threadCount) { // check if we can create thread
            pthread_mutex_lock(&mtx);
            threadCount++;
            pthread_mutex_unlock(&mtx);
            if (pthread_create(&task, NULL, threadedQS, &first) != 0) { // check if it was created successfully.
                pthread_mutex_lock(&mtx);
                threadCount--;
                pthread_mutex_unlock(&mtx);
                fprintf(stderr,"ERROR %d: %s (Failed to create thread %d).\n", errno, strerror(errno), threadCount);
                quickSort(&first); //if not do regular sort
                task = 0;
            }
        }
        else {// do it without sort
            quickSort(&first);                  /* sort the left partition	*/
        }
		
        SortParams second; second.array = array; second.left = i; second.right = right;
        //quickSort(&second);                 /* sort the right partition */
		if (maximumThreads > 0 && maximumThreads > threadCount) { // check if we can create thread
            pthread_mutex_lock(&mtx);
            threadCount++;
            pthread_mutex_unlock(&mtx);
            if (pthread_create(&task2, NULL, threadedQS, &second) != 0) { // check if it was created successfully.
                pthread_mutex_lock(&mtx);
                threadCount--;
                pthread_mutex_unlock(&mtx);
                fprintf(stderr,"ERROR %d: %s (Failed to create thread %d).\n", errno, strerror(errno), threadCount);
                quickSort(&second); //if not do regular sort
                task2 = 0;
            }
        }
        else {// do it without sort
            quickSort(&second);                  /* sort the left partition	*/
        }
        if(task) { // attempts to jooin thread
            if (pthread_join(task,NULL) != 0) {
                fprintf(stderr,"ERROR %d: %s (Failed to join thread %d).\n",errno, strerror(errno), threadCount);
            }
            else {
                pthread_mutex_lock(&mtx);
                threadCount--;
                pthread_mutex_unlock(&mtx);
            }
            task = 0;
        }
        if(task2) { // attempts to jooin thread
            if (pthread_join(task2,NULL) != 0) {
                fprintf(stderr,"ERROR %d: %s (Failed to join thread %d).\n",errno, strerror(errno), threadCount);
            }
            else {
                pthread_mutex_lock(&mtx);
                threadCount--;
                pthread_mutex_unlock(&mtx);
            }
            task2 = 0;
        }
    } else insertSort(array,i,j);           /* for a small range use insert sort */
}

/* user interface routine to set the number of threads sortT is permitted to use */

void setSortThreads(int count) {
    maximumThreads = count;
}

/* user callable sort procedure, sorts array of count strings, beginning at address array */

void sortThreaded(char** array, unsigned int count) {
    maximumThreads = (maximumThreads > count/10000)? count/10000:maximumThreads; // prevent some overhead by limiting 10k line per thread
    // never say to use the whole maximum defined thread
    SortParams parameters;
    parameters.array = array; parameters.left = 0; parameters.right = count - 1;
    quickSort(&parameters);
}

static void *threadedQS(void *arg) {
    SortParams *chunk = (SortParams*) arg;
    quickSort(chunk);
    return NULL;
}