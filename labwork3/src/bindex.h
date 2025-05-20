#ifndef BINDEX_H
#define BINDEX_H

#include "gf.h"
#include <stddef.h>

typedef struct BIndexNode
{
    gf2_12 b;
    size_t index;
    struct BIndexNode* left;
    struct BIndexNode* right;
} BIndexNode;

void freeBIndex(BIndexNode *node);
void printBIndexNode(BIndexNode *node);
BIndexNode* findBIndexNode(BIndexNode *baseNode, gf2_12 b);
int addBIndex(BIndexNode *baseNode, gf2_12 b, size_t index);
int addBIndexRecursive(BIndexNode *baseNode, BIndexNode *newNode);

#endif  // BINDEX_H