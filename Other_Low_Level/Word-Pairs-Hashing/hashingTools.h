#include "crc64.h"
#include "sTools.h"
#include <stdio.h>

#define BigInt unsigned long long
#define Pairs struct pairs

struct pairs {
  char *s;
  int occur;
  BigInt hash;
  Pairs *next;
};

void printTable(Pairs **htable, BigInt htSize, int printCollision);
Pairs **initTable (int tSize);
int insert(char *s, Pairs **hashTable, BigInt tableSize);
Pairs *searchFor (char *s, Pairs **hashTable, BigInt tableSize);
Pairs **growTable(Pairs **hashTable, BigInt *tableSize);
void rehashElements(Pairs **hashTable, Pairs **newTable, BigInt tableSize);
void freeHTable(Pairs **hashTable, BigInt tableSize);
Pairs *freeIndx(Pairs *ptrToIndx);
void safelyFree(Pairs *node, Pairs **hTable);
int checkTableNULL(Pairs **hashTable, BigInt tableSize);