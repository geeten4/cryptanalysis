#ifndef KEYPAIR_H
#define KEYPAIR_H

#include "gf.h"

typedef struct KeyPair
{
    gf2_12 key1;
    gf2_12 key2;
    struct KeyPair *left;
    struct KeyPair *right;
} KeyPair;

int addKeyPairRecursive(KeyPair *kp_base, KeyPair *kp_new);
int addKeyPair(KeyPair *kp_base, gf2_12 key1, gf2_12 key2);

#endif  // KEYPAIR_H
