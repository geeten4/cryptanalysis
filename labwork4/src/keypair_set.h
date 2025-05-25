#ifndef KEYPAIR_SET_H
#define KEYPAIR_SET_H

#include "gf.h"

typedef struct KeyPair {
    gf2_11 key1;
    gf2_11 key2;
} KeyPair;


typedef struct {
    KeyPair **data;
    int size;
    int capacity;
} KeyPairSet;


void print_keypair(KeyPair *kp);
KeyPair *create_keypair(gf2_11 key1, gf2_11 key2);
bool keypair_equal(KeyPair *kp1, KeyPair *kp2);
KeyPairSet* create_keypair_set();
void keypair_set_free(KeyPairSet *set);
int keypair_set_contains(KeyPairSet *set, KeyPair *kp);
void keypair_set_add(KeyPairSet *set, gf2_11 key1, gf2_11 key2);
void keypair_set_remove(KeyPairSet *set, KeyPair *kp);
void keypair_set_print(KeyPairSet *set);
void keypair_set_subtract(KeyPairSet *set1, KeyPairSet *set2);

#endif