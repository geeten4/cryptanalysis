#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "permutation.h"

static gf2_11 *permutation_data;
static bool is_permutation_data_initialized = false;
static gf2_11 *permutation_inverse_data;
static bool is_permutation_inverse_data_initialized = false;

static const char *PERMUTATION_PATH = "labwork4/data/permutation.txt";
static const char *PERMUTATION_INVERSE_PATH = "labwork4/data/permutation_inverse.txt";

gf2_11* initPermutation(FILE *file) {
    if (file == NULL) {
        printf("File loaded incorrectly.");
        return 0; 
    }
    char* line = NULL;
    size_t len = 0;

    gf2_11* p = malloc(FIELD_SIZE * sizeof(gf2_11));

    int x;    
    size_t counter = 0;
    while (getline(&line, &len, file) != -1) {
        sscanf(line, "%d", &x);
        p[counter] = (gf2_11) x;
        counter++;
    }

    return p;
}

gf2_11 permutation(gf2_11 x) {
    if(!is_permutation_data_initialized) {
        FILE *permutation_file = fopen(PERMUTATION_PATH, "r");
        permutation_data = initPermutation(permutation_file);
        fclose(permutation_file);
        is_permutation_data_initialized = true;
    }

    assert(x < FIELD_SIZE);
    return permutation_data[x];
}

gf2_11 permutation_inverse(gf2_11 x) {
    if(!is_permutation_inverse_data_initialized) {
        FILE *permutation_file = fopen(PERMUTATION_INVERSE_PATH, "r");
        permutation_inverse_data = initPermutation(permutation_file);
        fclose(permutation_file);
        is_permutation_inverse_data_initialized = true;
    }

    assert(x < FIELD_SIZE);
    return permutation_inverse_data[x];
}

void print_gf2_11_bytes(gf2_11 x) {
    assert(x < FIELD_SIZE);
    for (int i = 10; i >= 0 ; i--)
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

void print_double_gf2_11_bytes(double_gf2_11 x) {
    assert(x < FIELD_SIZE * FIELD_SIZE);
    for (int i = 21; i >= 0 ; i--)
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
