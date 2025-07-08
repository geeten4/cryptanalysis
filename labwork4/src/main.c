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
#include "keypair_set.h"

void secondExercise() {
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
    
    printf("Last remaining candidate for k_5 is: ");
    print_gf2_11_bytes(possible_keys->data[0]);
    printf("\n");
}

void firstExercise() {
    size_t roundCount = 5;
    
    // initialize keys
    gf2_11 *keys = malloc(roundCount * sizeof(gf2_11));
    for (size_t i = 0; i < roundCount; i++)
    {
        keys[i] = rand_gf2_11();
        printf("k_%ld: ", i);
        print_gf2_11_bytes(keys[i]);
        printf("\n");
    }

    KeyPairSet *possible_keys = create_keypair_set(), *keys_to_remove = create_keypair_set();

    // vector space containing combinantions of a_1, a_2, a_3
    gf2_11 *V = linear_span(linearly_indep_vectors());
    
    for (gf2_11 key3 = 0; key3 < FIELD_SIZE; key3++)
    {
        for (gf2_11 key4 = 0; key4 < FIELD_SIZE; key4++)
        {   
            if (check_keys(V, keys, roundCount, key3, key4)) {
                // first round loop, initialize possible_keys
                keypair_set_add(possible_keys, key3, key4);
            }
        }
    }

    printf("Possible key pairs remaining: %d\n", possible_keys->size);
    
    while (possible_keys->size > 1) {
        // generate new linear span of vectors a'_1, a'_2, a'_3
        V = linear_span(linearly_indep_vectors());
        for (size_t i = 0; i < possible_keys->size; i++)
        {
            KeyPair *kp = possible_keys->data[i];
            if (!check_keys(V, keys, roundCount, kp->key1, kp->key2)) {
                // using the guess for two last keys, decrypt last two rounds, if they are correct,
                // the partially decrypted pairs [V[j] || 0] must all sum to zero
                // if not, key guesses are not correct, remove then from possible_keys
                keypair_set_add(keys_to_remove, kp->key1, kp->key2);
            }
        }

        keypair_set_subtract(possible_keys, keys_to_remove);
        printf("Possible key pairs remaining: %d\n", possible_keys->size);
    }

    assert(possible_keys->size == 1);
    KeyPair *recovered_keypair = possible_keys->data[0];
    printf("Recovered k_3: ");
    print_gf2_11_bytes(recovered_keypair->key1);
    printf(", k_4: ");
    print_gf2_11_bytes(recovered_keypair->key2);
    printf("\n");
}

int main() {
    srand(time(NULL));   // Initialization, should only be called once.

    printf("First exercise: \n");
    firstExercise();

    printf("\nSecond exercise: \n");
    secondExercise();
}
