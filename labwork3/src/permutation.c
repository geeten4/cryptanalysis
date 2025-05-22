#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "permutation.h"

gf2_12 *permutation_F_data;
bool is_permutation_F_data_initialized = false;
gf2_12 *permutation_F_inverse_data;
bool is_permutation_F_inverse_data_initialized = false;

static const char *LABOWRK3_RADOM_PERMUTATION_PATH = "labwork3/data/random_permutation.txt";
static const char *LABOWRK3_RADOM_INVERSE_PERMUTATION_PATH = "labwork3/data/random_inverse_permutation.txt";


Node *addNodeRecursive(Node *baseNode, Node *newNode) {

    if ((baseNode->value < newNode->value) & (baseNode->right != 0))
        return addNodeRecursive(baseNode->right, newNode);
    else if ((baseNode->value < newNode->value) & (baseNode->right == 0))
        baseNode->right = newNode;
    else if ((baseNode->value > newNode->value) & (baseNode->left != 0))
        return addNodeRecursive(baseNode->left, newNode);
    else if ((baseNode->value > newNode->value) & (baseNode->left == 0))
        baseNode->left = newNode;
    else
        return baseNode;
    return 0;
}

Node *addNode(Node *baseNode, size_t i, gf2_12 value) {
    Node *newNode = malloc(sizeof(Node));
    newNode->value = value;
    newNode->i = i;
    newNode->left = 0;
    newNode->right = 0;

    return addNodeRecursive(baseNode, newNode);
}

void printNodes(Node *node) {
    if(node->left != 0) {
        printNodes(node->left);
    }
    printf("value: %d, i: %ld, \n", node->value, node->i);
    if (node->right != 0) {
        printNodes(node->right);
    }
}

bool isPermutation(gf2_12 (*permutation)(gf2_12)) {
    Node baseNode = {0, permutation(0), 0, 0};

    // check permutation_F is in fact a permutation
    for (size_t i = 1; i < 4096; i++)
    {
        gf2_12 f_i = permutation(i);
        Node *collisionNode = addNode(&baseNode, i, f_i);
        if(collisionNode != 0) {
            printf("colliding nodes: i: %ld, value: %d and i: %ld, value: %d", collisionNode->i, collisionNode->value, i, f_i);
            return false;
        }
    }
    return true;
}

void printBytes(gf2_12 x) {
    for (int i = sizeof(short int) * 8 - 1; i >= 0 ; i--)
    {
        if ((x >> i) % 2 == 0) {
            printf("%d", 0);
        } else {
            printf("%d", 1);
        }
        if (i % 4 == 0) {
            printf(" ");
        }
    }
}

bool isLinear(gf2_12 (*permutation)(gf2_12)) {
    // check it is non-linear
    gf2_12 f_i, f_j, f_ipj;
    for (size_t i = 0; i < 4096; i++)
    {
        for (size_t j = 0; j < 4096; j++)
        {
            if (i == j)
                continue;

            f_i = permutation(i);
            f_j = permutation(j);
            f_ipj = permutation((i+j) & 4096);

            if((f_i ^ f_j) != f_ipj) {
                return false;
            }
        }
    }
    return true;
}

gf2_12* initPermutation(FILE *file) {
    if (file == NULL) {
        printf("File loaded incorrectly.");
        return 0; 
    }
    char* line = NULL;
    size_t len = 0;

    gf2_12* p = malloc(4096 * sizeof(gf2_12));

    int x;    
    size_t counter = 0;
    while (getline(&line, &len, file) != -1) {
        sscanf(line, "%d", &x);
        p[counter] = (gf2_12) x;
        counter++;
    }

    return p;
}

gf2_12 permutation_F(gf2_12 x) {
    if(!is_permutation_F_data_initialized) {
        FILE *permutation_file = fopen(LABOWRK3_RADOM_PERMUTATION_PATH, "r");
        permutation_F_data = initPermutation(permutation_file);
        fclose(permutation_file);
        is_permutation_F_data_initialized = true;
    }

    assert(x < 4096);
    return permutation_F_data[x];
}

gf2_12 permutation_F_inverse(gf2_12 x) {
    if(!is_permutation_F_inverse_data_initialized) {
        FILE *permutation_file = fopen(LABOWRK3_RADOM_INVERSE_PERMUTATION_PATH, "r");
        permutation_F_inverse_data = initPermutation(permutation_file);
        fclose(permutation_file);
        is_permutation_F_inverse_data_initialized = true;
    }

    assert(x < 4096);
    return permutation_F_inverse_data[x];
}
