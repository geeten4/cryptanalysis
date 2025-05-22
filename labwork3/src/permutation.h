#ifndef PERMUTATION_H
#define PERMUTATION_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "gf.h"

gf2_12 *permutation_F_data;
bool is_permutation_F_data_initialized;
gf2_12 *permutation_F_inverse_data;
bool is_permutation_F_inverse_data_initialized;

typedef struct Node
{
    size_t i;
    gf2_12 value;
    struct Node *left;
    struct Node *right;
} Node;

Node *addNodeRecursive(Node *baseNode, Node *newNode);
Node *addNode(Node *baseNode, size_t i, gf2_12 value);
void printNodes(Node *node);
bool isPermutation(gf2_12 (*permutation)(gf2_12));
void printBytes(gf2_12 x);
bool isLinear(gf2_12 (*permutation)(gf2_12));
gf2_12* initPermutation(FILE *file);
gf2_12 permutation_F(gf2_12 x);
gf2_12 permutation_F_inverse(gf2_12 x);

#endif  // PERMUTATION_H