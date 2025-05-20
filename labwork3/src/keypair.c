#include <stdlib.h>

#include "keypair.h"

int addKeyPairRecursive(KeyPair *kp_base, KeyPair *kp_new) {
    int32_t indexNew = (kp_new->key1 << 12) ^ kp_new->key2;
    int32_t indexBase = (kp_base->key1 << 12) ^ kp_base->key2;

    if ((indexNew < indexBase) && (kp_base->left != 0)){
        return addKeyPairRecursive(kp_base->left, kp_new);
        }
    else if ((indexNew < indexBase) && (kp_base->left == 0)){
        kp_base->left = kp_new;
        }
    else if ((indexNew > indexBase) && (kp_base->right != 0)){
        return addKeyPairRecursive(kp_base->right, kp_new);
        }
    else if ((indexNew > indexBase) && (kp_base->right == 0)){
        kp_base->right = kp_new;
        }
    else{
        return 1;}
    return 0;
}

int addKeyPair(KeyPair *kp_base, gf2_12 key1, gf2_12 key2) {
    KeyPair *kp = malloc(sizeof(KeyPair));
    kp->key1 = key1;
    kp->key2 = key2;
    kp->left = 0;
    kp->right = 0;
    return addKeyPairRecursive(kp_base, kp);
}
