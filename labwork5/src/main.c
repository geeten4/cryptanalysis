#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "aes.h"
#include <time.h>
#include "gf.h"
#include "data.h"
#include "set.h"
#include "characteristic.h"
#include <stdbool.h>


void firstExercise(bool verbose) {
    // guesses the first column of the last key

    size_t round_count = 4;
    size_t key_count = round_count + 1;
    aes_state *keys = generate_keys(key_count);
    // print keys
    for (size_t i = 0; i < key_count; i++)
    {
        printf("k_%ld: ", i);
        print_aes_state(keys[i]);
    }

    // first store all possible keys, then check which are valid
    Set *possible_keys = create_set();
    aes_state plaintext, key_guess;

    // iterate over 2^16 possibilities for the first column of the last key
    uint32_t key_nibble_limit = 1 << 16;
    for (uint32_t key_nibbles = 0; key_nibbles < key_nibble_limit; key_nibbles++)
    {
        // choose a random plaintext, from this create the structure
        plaintext = random_aes_state();
        key_guess = random_aes_state();

        // initialize the last key guess 
        key_guess[0] = key_nibbles & 0xF;
        key_guess[1] = (key_nibbles >> 4) & 0xF;
        key_guess[2] = (key_nibbles >> 8) & 0xF;
        key_guess[3] = (key_nibbles >> 12) & 0xF;

        // given the guess for the first column of the last key, rework the last round
        // if the sum over the diagonal is zero, we have a candidate
        if (check_diagonal_zero(plaintext, key_guess, keys, round_count)) {
            set_add(possible_keys, (set_arg_t) ((key_guess[3] << 12)) ^ (key_guess[2] << 8) ^ (key_guess[1] << 4) ^ key_guess[0]);
        }
    }

    // iterate over the remainig possible keys
    Set *keys_to_remove = create_set();
    
    while (possible_keys->size > 1) {
        if (verbose)
            printf("possible_keys->size: %d\n", possible_keys->size);

        for (size_t i = 0; i < possible_keys->size; i++)
        {
            plaintext = random_aes_state();
            key_guess = random_aes_state();
            key_guess[0] = possible_keys->data[i] & 0xF;
            key_guess[1] = (possible_keys->data[i] >> 4) & 0xF;
            key_guess[2] = (possible_keys->data[i] >> 8) & 0xF;
            key_guess[3] = (possible_keys->data[i] >> 12) & 0xF;

            if (!check_diagonal_zero(plaintext, key_guess, keys, round_count))
                // the key guess does not sum to zero on the diagonal, remove it
                set_add(keys_to_remove, possible_keys->data[i]);

            if (verbose)
                printf("key_guess[0]: %d, key_guess[1]: %d, key_guess[2]: %d, key_guess[3]: %d\n", key_guess[0], key_guess[1], key_guess[2], key_guess[3]);
        }

        set_subtract(possible_keys, keys_to_remove);
    }

    printf("First column of the last key: %d, %d, %d, %d\n",
        (possible_keys->data[0]) & 0xF,
        (possible_keys->data[0] >> 4) & 0xF,
        (possible_keys->data[0] >> 8) & 0xF,
        (possible_keys->data[0] >> 12) & 0xF
    );
    
}

void secondExercise() {
    // guesses the diagonal of the first key
    size_t round_count = 4;
    size_t key_count = round_count + 1;
    aes_state *keys = generate_keys(key_count);

    // print keys
    for (size_t i = 0; i < key_count; i++)
    {
        printf("k_%ld: ", i);
        print_aes_state(keys[i]);
    }

    // first store all possible keys, then check which are valid
    // choose a random plaintext, from this create the structure
    Set *possible_keys = create_set(), *keys_to_remove = create_set();
    aes_state plaintext = random_aes_state(), key_guess = random_aes_state();

    // iterate over 2^16 possibilities for the diagonal of the first key
    for (uint32_t key_nibbles = 0; key_nibbles < (1 << 16); key_nibbles++)
    {
        // initialize the first key guess 
        key_guess[0] = key_nibbles & 0xF;
        key_guess[5] = (key_nibbles >> 4) & 0xF;
        key_guess[10] = (key_nibbles >> 8) & 0xF;
        key_guess[15] = (key_nibbles >> 12) & 0xF;

        if (check_first_column_zero(plaintext, key_guess, keys, round_count)) {
            set_add(possible_keys, (set_arg_t) ((key_guess[15] << 12)) ^ (key_guess[10] << 8) ^ (key_guess[5] << 4) ^ key_guess[0]);
        }
    }

    while (possible_keys->size > 1)
    {
        plaintext = random_aes_state();
        for (size_t i = 0; i < possible_keys->size; i++)
        {   
            key_guess[0] = possible_keys->data[i] & 0xF;
            key_guess[5] = (possible_keys->data[i] >> 4) & 0xF;
            key_guess[10] = (possible_keys->data[i] >> 8) & 0xF;
            key_guess[15] = (possible_keys->data[i] >> 12) & 0xF;

            if (!check_first_column_zero(plaintext, key_guess, keys, round_count)) {
                set_add(keys_to_remove, (set_arg_t) ((key_guess[15] << 12)) ^ (key_guess[10] << 8) ^ (key_guess[5] << 4) ^ key_guess[0]);
            }
        }
        set_subtract(possible_keys, keys_to_remove);
    }

    for (size_t i = 0; i < possible_keys->size; i++)
    {
        printf(
            "Diagonal of the first key: %d, %d, %d, %d\n",
            possible_keys->data[i]& 0xF,
            (possible_keys->data[i] >> 4) & 0xF,
            (possible_keys->data[i] >> 8) & 0xF,
            (possible_keys->data[i] >> 12) & 0xF
        );
    }
}

void combinedAttack(bool verbose) {
    // guesses the diagonal of the first key
    size_t round_count = 5;
    size_t key_count = round_count + 1;
    aes_state *keys = generate_keys(key_count);
    keys[key_count - 1][3] = 0;
    keys[key_count - 1][2] = 0;
    keys[key_count - 1][1] = 0;

    // print keys
    for (size_t i = 0; i < key_count; i++)
    {
        printf("k_%ld: ", i);
        print_aes_state(keys[i]);
    }

    // print the correct key guess for reference
    // for optimization, store the eight 4-bit values in a single uint32
    int keys_guess = eight_gf2_4_to_set_arg_t(
        keys[key_count - 1][3],
        keys[key_count - 1][2],
        keys[key_count - 1][1],
        keys[key_count - 1][0],
        keys[0][15],
        keys[0][10],
        keys[0][5],
        keys[0][0]
    );
    printf("correct keys guess: %u, last key guess: %u, first key guess: %u\n", keys_guess, keys_guess >> 16, keys_guess % (1 << 16));

    // first store all possible keys, then check which are valid
    // choose a random plaintext, from this create the structure
    Set *possible_keys = create_set();
    aes_state plaintext = random_aes_state(), first_key_guess = random_aes_state(), last_key_guess = random_aes_state();

    // iterate over 2^32 possibilities for the diagonal of the first key and the first column of the last key
    aes_state sum = create_aes_state(), helper = create_aes_state(), cipher = create_aes_state();
    for (uint64_t key_nibbles = 0; key_nibbles <  ((uint64_t) 1 << 32); key_nibbles++)
    {
        // initialize the first key guess 
        first_key_guess[0] = key_nibbles & 0xF;
        first_key_guess[5] = (key_nibbles >> 4) & 0xF;
        first_key_guess[10] = (key_nibbles >> 8) & 0xF;
        first_key_guess[15] = (key_nibbles >> 12) & 0xF;

        // initialize the last key guess 
        last_key_guess[0] = (key_nibbles >> 16) & 0xF;
        last_key_guess[1] = (key_nibbles >> 20) & 0xF;
        last_key_guess[2] = (key_nibbles >> 24) & 0xF;
        last_key_guess[3] = (key_nibbles >> 28) & 0xF;

        // first_key_guess = keys[0];
        // last_key_guess = keys[key_count - 1];

        for (size_t i = 0; i < 16; i++)
        {
            // set the form we want to have after the first round
            copy_aes_state(plaintext, helper);
            helper[0] = i;

            // rework the first round
            MixColumnsInv(helper);
            ShiftRowsInv(helper);
            SubBytes(helper);
            AddRoundKey(helper, first_key_guess);

            // now in helper we have the starting state we want to encrypt
            AES_encrypt(cipher, helper, keys, round_count);

            // now use the second key guess to rework the last round
            AddRoundKey(cipher, last_key_guess);
            MixColumnsInv(cipher);
            ShiftRowsInv(cipher);
            SubBytes(cipher);

            add_to_state(sum, cipher);
        }

        // check if the diagonal is zero
        if (sum[0] == 0 && sum[5] == 0 && sum[10] == 0 && sum[15] == 0) {
            uint32_t guess = eight_gf2_4_to_set_arg_t(
                last_key_guess[3],
                last_key_guess[2],
                last_key_guess[1],
                last_key_guess[0],
                first_key_guess[15],
                first_key_guess[10],
                first_key_guess[5],
                first_key_guess[0]
            );

            printf("key_nibbles=%ld, guess=%u, last_key_guess=%u, %% done: %f\n",
                key_nibbles,
                guess,
                guess >> 16,
                (float) key_nibbles / ((uint64_t) 1 << 32));
            set_add(possible_keys, guess);
        }
    }

    set_print(possible_keys);
}

int main() {
    /*
        aes_state as 4x4 8-bit value (128 bits total) too complex, 
        implemented 4x4 4-bit value (64 bits total)
    */

    srand(time(NULL));   // Initialization, should only be called once.

    printf("First exercise: \n");
    firstExercise(false);

    printf("\nSecond exercise: \n");
    secondExercise();

    // TODO: optimize - with current optimization takes ~30 hours to complete due to search over 2^32
    // memory handling is OK, probably needs a better implementation of AES state

    // TODO: for a correct guess of the last key and some (?) bits of the first key,
    // we get a better chance of the characteristic holding. Maybe do not iterate over
    // all of 2^32 possible key bits, but a few factors less
    printf("\nCombined attack: \n");
    combinedAttack(true);

}
