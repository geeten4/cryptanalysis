#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "characteristic.h"
#include "aes.h"


bool check_diagonal_zero(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count) {
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
        add_to_state(sum, cipher);
    }

    // check if the characteristic holds
    if (sum[0] == 0 && sum[5] == 0 && sum[10] == 0 && sum[15] == 0)
        return 1;
    else
        return 0;
}

bool check_first_column_zero(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count) {
    aes_state sum = create_aes_state(), helper, cipher;
    add_to_state(sum, sum);  // initialize to zero

    for (size_t i = 0; i < 16; i++)
    {
        // set the form we want to have after the first round
        helper = plaintext;
        helper[0] = i;

        // rework the first round
        helper = MixColumnsInv(helper);
        ShiftRowsInv(helper);
        SubBytes(helper);
        AddRoundKey(helper, key_guess);

        // now in helper we have the starting state we want to encrypt
        cipher = AES_encrypt(helper, keys, round_count);
        add_to_state(sum, cipher);
    }

    // check if the characteristic holds
    if (sum[0] == 0 && sum[1] == 0 && sum[2] == 0 && sum[3] == 0)
        return 1;
    else
        return 0;

}

bool check_combined_charac(aes_state plaintext, aes_state first_key_guess, aes_state last_key_guess, aes_state* keys, size_t round_count) {
    aes_state sum = create_aes_state(), helper, cipher;
    add_to_state(sum, sum);  // initialize to zero

    for (size_t i = 0; i < 16; i++)
    {
        // set the form we want to have after the first round
        helper = plaintext;
        helper[0] = i;

        // rework the first round
        helper = MixColumnsInv(helper);
        ShiftRowsInv(helper);
        SubBytes(helper);
        AddRoundKey(helper, first_key_guess);

        // now in helper we have the starting state we want to encrypt
        cipher = AES_encrypt(helper, keys, round_count);

        // now use the second key guess to rework the last round
        AddRoundKey(cipher, last_key_guess);
        cipher = MixColumnsInv(cipher);
        ShiftRowsInv(cipher);
        SubBytes(cipher);

        add_to_state(sum, cipher);
    }

    // check if the diagonal is zero
    if (sum[0] == 0 && sum[5] == 0 && sum[10] == 0 && sum[15] == 0)
        return true;
    else
        return false;
}
