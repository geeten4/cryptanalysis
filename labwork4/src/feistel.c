#include <stdlib.h>
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
    gf2_11 left = double_left(message), right = double_right(message), helper;
    for (size_t i = 0; i < roundCount; i++)
    {
        helper = right;
        right = left ^ roundFunction(right, keys[i]);
        left = helper;
    }

    assert(left < (1 << 11));
    assert(right < (1 << 11));

    return create_double(left, right);
}

double_gf2_11 Feistel_decrypt(double_gf2_11 cipher, gf2_11 *keys, size_t roundCount) {
    // roundCount == len(keys)
    gf2_11 left = double_left(cipher), right = double_right(cipher), helper;
    for (int i = roundCount - 1; i >= 0; i--)
    {
        helper = left;
        left = right ^ roundFunction(left, keys[i]);
        right = helper;
    }

    assert(left < (1 << 11));
    assert(right < (1 << 11));
    return create_double(left, right);
}

bool check_keys(gf2_11* V, gf2_11* keys, size_t roundCount, gf2_11 key3, gf2_11 key4) {
    /*
        given a cipher c, decrypts the last two Feistel rounds using key3 and key4,
        checks if the sum of the decrypted left sides sums to zero, then returns true

        sum_{v \in V} f(v + x) = sum_{v \in V} f(v + f(0 + k_1) + k_2) = 0
        where f(v + f(0 + k_1) + k_2) is the left value when encrypting (v, 0)
        before the 4th XOR
    */
   gf2_11 v_checksum = 0;
   for (size_t j = 0; j < 8; j++)
   {
        // v \in V, v = V[j]
        // request encryption
        double_gf2_11 c = Feistel_encrypt(create_double(V[j], 0), keys, roundCount);
        gf2_11 left = double_left(c), right = double_right(c), helper;

        // rework last two rounds of Feistel
        helper = left;
        left = right ^ roundFunction(left, key4);
        right = helper;
        
        helper = left;
        left = right ^ roundFunction(left, key3);
        right = helper;

        v_checksum ^= left;
   }
   return v_checksum == 0;
}

gf2_11 *linear_span(gf2_11 *a) {
    /*
        returns all linear combinations of the first three array elements
    */
    gf2_11 *V = malloc(sizeof(gf2_11) * 8);
    V[0] = 0;
    V[1] = a[0];
    V[2] = a[1];
    V[3] = a[2];
    V[4] = a[0] ^ a[1];
    V[5] = a[0] ^ a[2];
    V[6] = a[1] ^ a[2];
    V[7] = a[0] ^ a[1] ^ a[2];
    return V;
}

gf2_11 *linearly_indep_vectors(int count) {
    /*
        returns three linearly independent vectors
    */
    gf2_11 *a = malloc(sizeof(gf2_11) * 3);
    while (true) {
        a[0] = rand_gf2_11(), a[1] = rand_gf2_11(), a[2] = rand_gf2_11();
        // check if they are linearly independent
        if ((a[0] != a[1]) && (a[0] != a[2]) && (a[1] != a[2]) && ((a[0] ^ a[1]) != a[2]))
        break;
    }
    return a;
}
