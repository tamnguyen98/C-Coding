#include "sTools.h"
#include <stdio.h>
// return a copy of the concat string (s1+s2)
char *concat(char* s1, char* s2) {
    //s2 can not be null
    int isS2Empty = (strcmp(s2,"") == 0)? 1:0;
    int len = strlen(s1) + strlen(s2) + (isS2Empty? 1:2);
    char *ns = malloc(len*sizeof(char)); // if there are two words, add space between them
    for (int i = 0; i < len; i++) ns[i] = '\0';
    strcat(ns, s1);
    if (!isS2Empty) strcat(ns, " "); // add the space
    strcat(ns, s2);
    return ns;
}

// convert a string to lowecase
// thought I would need this at first
char *strToLower(char *s) {
    if (s == NULL)
        return s;
    char *c;
    for (c = s;*c != '\0'; c++) {
        *c = (char)*c|32;
    }
    return s; // returning encase we dont want to use an extra line just to convert
}
