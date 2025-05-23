#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stddef.h>

#include "permutation.h"
#include "feistel.h"
#include "gf.h"
#include "set.h"

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
    
    double_gf2_11 message, *ciphers;
    Set *possible_keys = create_set(), *keys_to_remove = create_set();
    gf2_11 c, c_i_L, c_i_R, c_j_L, c_j_R, key;

    // initialize all possible keys
    for (gf2_11 i = 0; i < FIELD_SIZE; i++)
        set_add(possible_keys, i);
    
    while (possible_keys->size > 1)
    {
        c = rand_gf2_11();
        // compute all ciphers for messages of the form [i, c], 0 <= i < FIELD_SIZE
        ciphers = malloc(sizeof(double_gf2_11) * FIELD_SIZE);
        for (gf2_11 i = 0; i < FIELD_SIZE; i++)
        {
            message = create_double(i, c);
            ciphers[i] = Feistel_encrypt(message, keys, roundCount);
        }

        // go through all the pairs 0 <= i, j < FIELD_SIZE, check if 
        // let c_i = Feistel([i, c]), c_j = Feistel([j, c])
        // and [i, c] ^ [j, c] == [i ^ j, 0] == [\alpha, 0]
        // check if also c_i = [c_i_L, c_i_R], c_j = [c_j_L, c_j_R] satisfies
        // c_i_L ^ c_j_L == \alpha s.t. c_i ^ c_j == [\alpha, X]
        // note the switch in the last round as well
        for (gf2_11 i = 0; i < FIELD_SIZE; i++)
        {
            for (gf2_11 j = 0; j < i; j++)
            {
                if (i == j)
                    continue;
                
                c_i_L = double_left(ciphers[i]);
                c_j_L = double_left(ciphers[j]);

                if ((c_i_L ^ c_j_L) != (i ^ j))
                    continue;

                c_i_R = double_right(ciphers[i]);
                c_j_R = double_right(ciphers[j]);

                // we matched the desired differential
                // work the last round of Feistel backwards
                // check if the impossible characteristic appears
                for (size_t i = 0; i < possible_keys->size; i++)
                {
                    key = possible_keys->data[i];
                    if ((c_i_R ^ roundFunction(c_i_L, key)) == (c_j_R ^ roundFunction(c_j_L, key)))
                        set_add(keys_to_remove, key);
                }

                set_subtract(possible_keys, keys_to_remove);

            }
            
        }
        printf("Remaining possible key count: %d\n", possible_keys->size);
    }

    printf("Last remaining possible key is %d, key_5 is %d\n", possible_keys->data[0], keys[roundCount - 1]);
}
