#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stddef.h>

#include "permutation.h"
#include "feistel.h"
#include "gf.h"

int main() {
    srand(time(NULL));   // Initialization, should only be called once.

    size_t roundCount = 6;
    // initialize keys
    gf2_11 *keys = malloc(roundCount * sizeof(gf2_11));
    for (size_t i = 0; i < roundCount; i++)
    {
        keys[i] = rand_gf2_11();
        printf("k_%ld: ", i);
        print_gf2_11_bytes(keys[i]);
        printf("\n");
    }
    
    double_gf2_11 message = 123456;
    printf("message: ");
    print_double_gf2_11_bytes(message);
    printf("\n");
    double_gf2_11 cipher = Feistel_encrypt(message, keys, roundCount);
    printf("cipher: ");
    print_double_gf2_11_bytes(cipher);
    printf("\n");
    double_gf2_11 decipher = Feistel_decrypt(cipher, keys, roundCount);
    printf("decipher: ");
    print_double_gf2_11_bytes(decipher);
    printf("\n");
    return 0;
}
