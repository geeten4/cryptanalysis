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
    // printf("\nFeistel_encrypt:\n");
    gf2_11 left = message >> 11, right = message % (1 << 11), helper;
    for (size_t i = 0; i < roundCount; i++)
    {
        // printf("Feistel_encrypt_%ld: left: ", i);
        // print_gf2_11_bytes(left);
        // printf("\nFeistel_encrypt_%ld: right: ", i);
        // print_gf2_11_bytes(right);
        // printf("\n");
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
    // printf("\nFeistel_decrypt:\n");
    gf2_11 left = cipher >> 11, right = cipher % (1 << 11), helper;
    for (int i = roundCount - 1; i >= 0; i--)
    {
        // printf("Feistel_decrypt_%d: left: ", i);
        // print_gf2_11_bytes(left);
        // printf("\nFeistel_decrypt_%d: right: ", i);
        // print_gf2_11_bytes(right);
        // printf("\n");
        helper = left;
        left = right ^ roundFunction(left, keys[i]);
        right = helper;
    }

    assert(left < (1 << 11));
    assert(right < (1 << 11));
    return (double_gf2_11) (left << 11) ^ right;
}