#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "keypair_set.h"
#include "gf.h"
#include "permutation.h"

/*
 * Implements a set structure using a simple dynamic array
 */

#define INITIAL_CAPACITY 1

void print_keypair(KeyPair *kp) {
    printf("{ key1: ");
    print_gf2_11_bytes(kp->key1);
    printf(", key2: ");
    print_gf2_11_bytes(kp->key2);
    printf(" }\n");
}

KeyPair *create_keypair(gf2_11 key1, gf2_11 key2) {
    KeyPair *kp = malloc(sizeof(KeyPair*));
    kp->key1 = key1;
    kp->key2 = key2;
    return kp;
}

bool keypair_equal(KeyPair *kp1, KeyPair *kp2) {
    return (kp1->key1 == kp2->key1) && (kp1->key2 == kp2->key2);
}

KeyPairSet* create_keypair_set() {
    KeyPairSet *set = malloc(sizeof(KeyPairSet));
    set->data = malloc(sizeof(KeyPair*) * INITIAL_CAPACITY);
    set->size = 0;
    set->capacity = INITIAL_CAPACITY;
    return set;
}

void keypair_set_free(KeyPairSet *set) {
    free(set->data);
    free(set);
}

int keypair_set_contains(KeyPairSet *set, KeyPair *kp) {
    for (int i = 0; i < set->size; i++) {
        if (keypair_equal(set->data[i], kp))
            return 1;
    }
    return 0;
}

void keypair_set_add(KeyPairSet *set, gf2_11 key1, gf2_11 key2) {
    KeyPair *kp = create_keypair(key1, key2);

    if (keypair_set_contains(set, kp)) {
        free(kp);
        return;
    }

    if (set->size == set->capacity) {
        set->capacity *= 2;
        set->data = realloc(set->data, sizeof(KeyPair*) * set->capacity);
    }

    set->data[set->size++] = kp;
}

void keypair_set_remove(KeyPairSet *set, KeyPair *kp) {
    for (int i = 0; i < set->size; i++) {
        if (keypair_equal(set->data[i], kp)) {
            set->data[i] = set->data[--set->size]; // Replace with last element
            return;
        }
    }
}

void keypair_set_print(KeyPairSet *set) {
    printf("{ \n");

    for (int i = 0; i < set->size; i++)
        print_keypair(set->data[i]);

    printf("\n}\n");
}

void keypair_set_subtract(KeyPairSet *set1, KeyPairSet *set2) {
    for (size_t i = 0; i < set2->size; i++)
        keypair_set_remove(set1, set2->data[i]);
}