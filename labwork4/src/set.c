#include <stdio.h>
#include <stdlib.h>
#include "set.h"

/*
 * Implements a set structure using a simple dynamic array
 */

#define INITIAL_CAPACITY 1

Set* create_set() {
    Set *set = malloc(sizeof(Set));
    set->data = malloc(sizeof(gf2_11) * INITIAL_CAPACITY);
    set->size = 0;
    set->capacity = INITIAL_CAPACITY;
    return set;
}

void set_free(Set *set) {
    free(set->data);
    free(set);
}

int set_contains(Set *set, gf2_11 x) {
    for (int i = 0; i < set->size; i++) {
        if (set->data[i] == x)
            return 1;
    }
    return 0;
}

void set_add(Set *set, gf2_11 x) {
    if (set_contains(set, x)) return;

    if (set->size == set->capacity) {
        set->capacity *= 2;
        set->data = realloc(set->data, sizeof(gf2_11) * set->capacity);
    }

    set->data[set->size++] = x;
}

void set_remove(Set *set, gf2_11 x) {
    for (int i = 0; i < set->size; i++) {
        if (set->data[i] == x) {
            set->data[i] = set->data[--set->size]; // Replace with last element
            return;
        }
    }
}

void set_print(Set *set) {
    printf("{ ");

    for (int i = 0; i < set->size; i++)
        printf("%d, ", set->data[i]);

    printf("}\n");
}

void set_subtract(Set *set1, Set *set2) {
    for (size_t i = 0; i < set2->size; i++)
        set_remove(set1, set2->data[i]);
}