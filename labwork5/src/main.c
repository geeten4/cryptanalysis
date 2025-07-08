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
    size_t round_count = 6;
    size_t key_count = round_count + 1;
    aes_state *keys = generate_keys(key_count);

    printf("=== Combined Attack (6 Rounds) ===\n");
    for (size_t i = 0; i < key_count; i++) {
        printf("k_%ld: ", i);
        print_aes_state(keys[i]);
    }

    Set *possible_keys = create_set(); // Each entry is 32 bits: [first_key_diag (16) | last_key_col (16)]
    aes_state plaintext = random_aes_state();
    aes_state first_key_guess = random_aes_state();
    aes_state last_key_guess = random_aes_state();

    for (uint32_t diag = 0; diag < (1 << 16); diag++) {
        // Fill diagonal of first key
        first_key_guess[0] = diag & 0xF;
        first_key_guess[5] = (diag >> 4) & 0xF;
        first_key_guess[10] = (diag >> 8) & 0xF;
        first_key_guess[15] = (diag >> 12) & 0xF;

        for (uint32_t col = 0; col < (1 << 16); col++) {
            // Fill column of last key
            last_key_guess[0] = col & 0xF;
            last_key_guess[1] = (col >> 4) & 0xF;
            last_key_guess[2] = (col >> 8) & 0xF;
            last_key_guess[3] = (col >> 12) & 0xF;

            if (check_first_column_zero(plaintext, first_key_guess, keys, round_count) &&
                check_diagonal_zero(plaintext, last_key_guess, keys, round_count)) {
                
                set_arg_t key_combined = (((set_arg_t)diag) << 16) | (set_arg_t)col;
                set_add(possible_keys, key_combined);
            }
        }
    }

    Set *keys_to_remove = create_set();

    while (possible_keys->size > 1) {
        plaintext = random_aes_state();

        for (size_t i = 0; i < possible_keys->size; i++) {
            uint32_t diag = possible_keys->data[i] >> 16;
            uint32_t col = possible_keys->data[i] & 0xFFFF;

            // Reconstruct guesses
            first_key_guess[0] = diag & 0xF;
            first_key_guess[5] = (diag >> 4) & 0xF;
            first_key_guess[10] = (diag >> 8) & 0xF;
            first_key_guess[15] = (diag >> 12) & 0xF;

            last_key_guess[0] = col & 0xF;
            last_key_guess[1] = (col >> 4) & 0xF;
            last_key_guess[2] = (col >> 8) & 0xF;
            last_key_guess[3] = (col >> 12) & 0xF;

            if (!(check_first_column_zero(plaintext, first_key_guess, keys, round_count) &&
                  check_diagonal_zero(plaintext, last_key_guess, keys, round_count))) {
                set_add(keys_to_remove, possible_keys->data[i]);
            }

            if (verbose) {
                printf("diag: %04x, col: %04x\n", diag, col);
            }
        }

        set_subtract(possible_keys, keys_to_remove);
        keys_to_remove->size = 0;

        if (verbose)
            printf("Remaining keys: %d\n", possible_keys->size);
    }

    if (possible_keys->size == 1) {
        uint32_t diag = possible_keys->data[0] >> 16;
        uint32_t col = possible_keys->data[0] & 0xFFFF;

        printf("Final key guess found:\n");
        printf("Diagonal of first key: %d, %d, %d, %d\n", diag & 0xF, (diag >> 4) & 0xF, (diag >> 8) & 0xF, (diag >> 12) & 0xF);
        printf("First column of last key: %d, %d, %d, %d\n", col & 0xF, (col >> 4) & 0xF, (col >> 8) & 0xF, (col >> 12) & 0xF);
    } else {
        printf("No unique key candidate found.\n");
    }
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

    // TODO: optimize
    // combinedAttack(true);

}
