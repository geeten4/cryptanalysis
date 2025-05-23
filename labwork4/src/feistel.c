#include <assert.h>
#include <stddef.h>

#include "feistel.h"
#include "gf.h"
#include "permutation.h"


gf2_11 roundFunction(gf2_11 x, gf2_11 key) {
    return permutation(x ^ key);
}

double_gf2_11 Feistel_encrypt(double_gf2_11 message, gf2_11 *keys, size_t roundCount) {
    // includes last swap
    // roundCount == len(keys)
    gf2_11 left = message >> 11, right = message % (1 << 11), helper;
    for (size_t i = 0; i < roundCount; i++)
    {
        helper = right;
        right = left ^ roundFunction(right, keys[i]);
        left = helper;
    }

    assert(left < (1 << 11));
    assert(right < (1 << 11));
    return (double_gf2_11) (left << 11) ^ right;
}

double_gf2_11 Feistel_decrypt(double_gf2_11 cipher, gf2_11 *keys, size_t roundCount) {
    // roundCount == len(keys)
    gf2_11 left = cipher >> 11, right = cipher % (1 << 11), helper;
    for (int i = roundCount - 1; i >= 0; i--)
    {
        helper = left;
        left = right ^ roundFunction(left, keys[i]);
        right = helper;
    }

    assert(left < (1 << 11));
    assert(right < (1 << 11));
    return (double_gf2_11) (left << 11) ^ right;
}