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


void firstExercise() {
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
        
        if (check_diagonal_zero(plaintext, key_guess, keys, round_count)) {
            set_add(possible_keys, (set_arg_t) ((key_guess[3] << 12)) ^ (key_guess[2] << 8) ^ (key_guess[1] << 4) ^ key_guess[0]);
        }
    }

    // iterate over the remainig possible keys
    Set *keys_to_remove = create_set();
    
    while (possible_keys->size > 1) {
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
                set_add(keys_to_remove, possible_keys->data[i]);

            printf("key_guess[0]: %d, key_guess[1]: %d, key_guess[2]: %d, key_guess[3]: %d\n", key_guess[0], key_guess[1], key_guess[2], key_guess[3]);
        }
        if (possible_keys->size == 2) {
            set_print(possible_keys);
            // return;
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

    // iterate over 2^16 possibilities for the first column of the last key
    for (uint32_t key_nibbles = 0; key_nibbles < (1 << 16); key_nibbles++)
    {
        // initialize the last key guess 
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

void combinedAttack() {
    size_t round_count = 5;
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
    // Set *possible_keys = create_set(), *keys_to_remove = create_set();
    Set *possible_keys = create_set();
    aes_state plaintext = random_aes_state(), first_key_guess = random_aes_state(), last_key_guess = random_aes_state();
    set_arg_t possible_key;

    first_key_guess[0] = keys[0][0];
    first_key_guess[5] = keys[0][5];
    first_key_guess[10] = keys[0][10];
    first_key_guess[15] = keys[0][15];

    last_key_guess[0] = keys[5][0];
    last_key_guess[1] = keys[5][1];
    last_key_guess[2] = keys[5][2];
    last_key_guess[3] = keys[5][3];

    if (check_combined_charac(plaintext, first_key_guess, last_key_guess, keys, round_count)) {
        possible_key = (first_key_guess[15] << 28) ^ (first_key_guess[10] << 24) ^ (first_key_guess[5] << 20) ^ (first_key_guess[0] << 16);
        possible_key ^= ((last_key_guess[15] << 12)) ^ (last_key_guess[10] << 8) ^ (last_key_guess[5] << 4) ^ last_key_guess[0];
        set_add(possible_keys, possible_key);
    }

    // // iterate over 2^16 possibilities for the first column of the last key
    // for (uint32_t key_nibbles = 0; key_nibbles < (1 << 16); key_nibbles++)
    // {
    //     // initialize the last key guess 
    //     key_guess[0] = key_nibbles & 0xF;
    //     key_guess[5] = (key_nibbles >> 4) & 0xF;
    //     key_guess[10] = (key_nibbles >> 8) & 0xF;
    //     key_guess[15] = (key_nibbles >> 12) & 0xF;

    //     if (check_first_column_zero(plaintext, key_guess, keys, round_count)) {
    //         set_add(possible_keys, (set_arg_t) ((key_guess[15] << 12)) ^ (key_guess[10] << 8) ^ (key_guess[5] << 4) ^ key_guess[0]);
    //     }
    // }
    
}

int main() {
    /*
        aes_state as 4x4 8-bit value (128 bits total) too complex, 
        implemented 4x4 4-bit value (64 bits total)
    */

    srand(time(NULL));   // Initialization, should only be called once.
    
    firstExercise();
    
    // secondExercise();

    // combinedAttack();

    // size_t round_count = 5;
    // size_t key_count = round_count + 1;
    // aes_state *keys = generate_keys(key_count);

    // aes_state state = create_aes_state();
    // state = random_aes_state();
    // state[0] = 1;
    // state[5] = 1;
    // state[10] = 1;
    // state[15] = 1;

    // print_aes_state(state);
    // aes_state cipher = AES_encrypt(state, keys, round_count);
    // printf("cipher: ");
    // print_aes_state(cipher);
    // aes_state deciphered = AES_decrypt(cipher, keys, round_count);
    // printf("deciphered: ");
    // print_aes_state(deciphered);

}
