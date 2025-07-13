#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "characteristic.h"
#include "aes.h"
#include "set.h"


bool check_diagonal_zero(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count) {
    aes_state sum = create_aes_state(), cipher = create_aes_state();
    add_to_state(sum, sum);  // initialize to zero 

    for (size_t i = 0; i < 16; i++) {
        // create the structure
        plaintext[0] = i;

        // encrypt
        AES_encrypt(cipher, plaintext, keys, round_count);

        // rework last round
        AddRoundKey(cipher, key_guess);
        MixColumnsInv(cipher);
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
    aes_state sum = create_aes_state(), helper = create_aes_state(), cipher = create_aes_state();
    add_to_state(sum, sum);  // initialize to zero

    for (size_t i = 0; i < 16; i++)
    {
        // set the form we want to have after the first round
        copy_aes_state(plaintext, helper);
        helper[0] = i;

        // rework the first round
        MixColumnsInv(helper);
        ShiftRowsInv(helper);
        SubBytes(helper);
        AddRoundKey(helper, key_guess);

        // now in helper we have the starting state we want to encrypt
        AES_encrypt(cipher, helper, keys, round_count);
        add_to_state(sum, cipher);
    }

    // check if the characteristic holds
    if (sum[0] == 0 && sum[1] == 0 && sum[2] == 0 && sum[3] == 0)
        return 1;
    else
        return 0;

}


set_arg_t four_gf2_4_to_set_arg_t(gf2_4 a_1, gf2_4 a_2, gf2_4 a_3, gf2_4 a_4) {
    return (a_1 << 12) ^ (a_2 << 8) ^ (a_3 << 4) ^ a_4;
}

set_arg_t eight_gf2_4_to_set_arg_t(gf2_4 a_1, gf2_4 a_2, gf2_4 a_3, gf2_4 a_4, gf2_4 a_5, gf2_4 a_6, gf2_4 a_7, gf2_4 a_8) {
    a_1 = a_1 & 0xF;
    a_2 = a_2 & 0xF;
    a_3 = a_3 & 0xF;
    a_4 = a_4 & 0xF;
    a_5 = a_5 & 0xF;
    a_6 = a_6 & 0xF;
    a_7 = a_7 & 0xF;
    a_8 = a_8 & 0xF;
    return (a_1 << 28) ^ (a_2 << 24) ^ (a_3 << 20) ^ (a_4 << 16) ^ (a_5 << 12) ^ (a_6 << 8) ^ (a_7 << 4) ^ a_8;
}
