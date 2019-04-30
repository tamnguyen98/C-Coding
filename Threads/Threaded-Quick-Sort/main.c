#include "sort.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <err.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>	/* for uint64 definition */
#define DICT_MAX_WORD_LEN	256
#define ARR_COUNT 1000000
#define BILLION 1000000000L

char *getLine(FILE *fp);
void timeNoThreads(char **origList, int size);
void timeThread(char **origList, int size);
void timeQsort(char **origList, int size);
int compare_function(const void *name1, const void *name2);


int end = 0;
void main(int argc, char **argv) {
    if (argc != 2) return;
    char **list = (char**) malloc(sizeof(char*) * ARR_COUNT);
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Error opening file %s\n",argv[1]);
        exit(-1);
    }
    int i = 0;
    while (end == 0 && i< ARR_COUNT) {
            list[i++] = getLine(fp);
    }
    timeQsort(list,i);
    timeNoThreads(list,i);
    timeThread(list, i);
    for (int n = ARR_COUNT-1; n >= 0; n--) {
        free(list[n]);
    }
    fclose(fp);
    free(list);
}

char *getLine(FILE *fp) {
    char buff[500] = {'\n'};
    char *ret  = NULL;
    int i = 0;
    for (i=0; ((buff[i] = fgetc(fp)) != '\0' && buff[i] !='\n') && i < 500; i++) {
        if (buff[i] == EOF) {
            end = 1;
            break;
        }
    }
    if (i-1 < 500) {
        buff[i] = '\n';
    }
    else
        buff[499] = '\n';
    return strdup(buff);
}

int compare_function(const void *name1, const void *name2)
{
    const char *aa = *(const char **) name1;
    const char *bb = *(const char **) name2;
    return strcmp(aa, bb);
}

void printList(char **list, int size) {
    if (list == NULL) return;
    for (int i = 0; i < size; i++)
        printf("%s\n", list[i]);
}

void timeNoThreads(char **origList, int size) {
    char **list1 = malloc(sizeof(char*)*size);
    for (int i = 0; i < size; i++) {
        list1[i] = origList[i];
    }
    uint64_t diff;
	struct timespec start, end;
    setSortThreads(0);
    printf("0 threads:\n");
    clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */
    sortThreaded(list1, size-1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	printf("%llu nano seconds\n", (long long unsigned int) diff);
    
    free(list1);

}

void timeThread(char **origList, int size) {
    char **list1 = malloc(sizeof(char*)*size);
    for (int i = 0; i < size; i++) {
        list1[i] = origList[i];
    }
    uint64_t diff;
	struct timespec start, end;
    setSortThreads(10);
    printf("10 threads:\n");
    clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */
    sortThreaded(list1, size-1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	printf("%llu nano seconds\n", (long long unsigned int) diff);
    //printList(list1, size);
    
    free(list1);

}

void timeQsort(char **origList, int size) {
    char **list1 = malloc(sizeof(char*)*size);
    for (int i = 0; i < size; i++) {
        list1[i] = origList[i];
    }
    uint64_t diff;
	struct timespec start, end;
    printf("qsort threads:\n");
    clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */
    qsort(list1, size, sizeof(list1[0]), compare_function);
    clock_gettime(CLOCK_MONOTONIC, &end);
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	printf("%llu nano seconds\n", (long long unsigned int) diff);

    free(list1);

}