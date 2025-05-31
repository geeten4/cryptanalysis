#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "characteristic.h"
#include "aes.h"


bool check_first_column_charac(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count) {
    aes_state sum = create_aes_state();
    add_to_state(sum, sum);  // initialize to zero 

    for (size_t i = 0; i < 16; i++) {
        // create the structure
        plaintext[0] = i;

        // encrypt
        aes_state cipher = AES_encrypt(plaintext, keys, round_count);

        // rework last round
        AddRoundKey(cipher, key_guess);
        cipher = MixColumnsInv(cipher);
        ShiftRowsInv(cipher);
        SubBytes(cipher);

        // add to sum
        AddRoundKey(sum, cipher);
    }

    // check if the characteristic holds
    if (sum[0] == 0 && sum[5] == 0 && sum[10] == 0 && sum[15] == 0)
        return 1;
    else
        return 0;
}

bool check_all_zero_charac(aes_state plaintext, aes_state *keys, size_t round_count) {
    aes_state sum = create_aes_state(), random_state = create_aes_state(), helper, cipher;
    add_to_state(sum, sum);  // initialize to zero

    for (size_t i = 0; i < 16; i++)
    {
        // set the form we want to have after the first round
        helper = random_state;
        helper[0] = i;
    
        // rework the first round
        helper = MixColumnsInv(helper);
        ShiftRowsInv(helper);
        SubBytes(helper);
        AddRoundKey(helper, keys[0]);

        // now in helper we have the starting state we want to encrypt
        cipher = AES_encrypt(helper, keys, round_count);
        add_to_state(sum, cipher);
    }

    print_aes_state(sum);
}