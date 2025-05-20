#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "bindex.h"
#include "gf.h"


int addBIndexRecursive(BIndexNode *baseNode, BIndexNode *newNode) {

    if ((baseNode->b <= newNode->b) & (baseNode->right != 0))
        return addBIndexRecursive(baseNode->right, newNode);
    else if ((baseNode->b <= newNode->b) & (baseNode->right == 0))
        baseNode->right = newNode;
    else if ((baseNode->b > newNode->b) & (baseNode->left != 0))
        return addBIndexRecursive(baseNode->left, newNode);
    else if ((baseNode->b > newNode->b) & (baseNode->left == 0))
        baseNode->left = newNode;
    else
        return 1;
    return 0;
}

int addBIndex(BIndexNode *baseNode, gf2_12 b, size_t index) {
    BIndexNode *newNode = malloc(sizeof(BIndexNode));
    newNode->b = b;
    newNode->index = index;
    newNode->left = 0;
    newNode->right = 0;

    return addBIndexRecursive(baseNode, newNode);
}

BIndexNode* findBIndexNode(BIndexNode *baseNode, gf2_12 b) {
    BIndexNode *node = baseNode;

    while (true) {
        if (node->b == b) {
            return node;
        } else if ((node->b < b) & (node->right == 0)) {
            return 0;
        } else if ((node->b < b) & (node->right != 0)) {
            node = node->right;
            continue;
        } else if ((node->b > b) & (node->left == 0)) {
            return 0;
        } else if ((node->b > b) & (node->left != 0)) {
            node = node->left;
            continue;
        } else {
            return 0;
        }
    }
}

void printBIndexNode(BIndexNode *node) {
    if(node->left != 0) {
        printBIndexNode(node->left);
    }
    printf("b: %d, index: %ld, left: %ld, right: %ld\n", node->b, node->index, (uintptr_t) node->left, (uintptr_t) node->right);
    if (node->right != 0) {
        printBIndexNode(node->right);
    }
}

void freeBIndex(BIndexNode *node) {
    // printf("\nfreeBIndex\n");
    if (node->left != 0) {
        freeBIndex(node->left);
        // printf("free node->left: %ld\n", (uintptr_t) node->left);
        free(node->left);
    }
    
    if (node->right != 0) {
        freeBIndex(node->right);
        // printf("free node->right: %ld\n", (uintptr_t) node->right);
        free(node->right);
    }
}
