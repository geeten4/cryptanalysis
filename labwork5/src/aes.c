#include <stdlib.h>
#include <stdio.h>

#include "aes.h"


// NOTE: is its own inverse
//                                0, 1, 2,  3,  4,  5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15
static const uint8_t S_Box[16] = {0, 1, 9, 14, 13, 11, 7, 6, 15, 2, 12,  5, 10,  4,  3,  8};

aes_state random_aes_state() {
    aes_state state = malloc(16 * sizeof(uint8_t));
    for (size_t j = 0; j < 16; j++)
    {
        state[j] = (uint8_t) rand();
    }
    return state;
}

aes_state *generate_keys(size_t key_count) {
    aes_state *keys = malloc(key_count * sizeof(aes_state*));
    for (size_t i = 0; i < key_count; i++)
    {
        keys[i] = random_aes_state();
    }
    return keys;
}

aes_state create_aes_state() {
    return malloc(sizeof(uint8_t) * 16);
}

void copy_aes_state(aes_state from, aes_state to) {
    for (size_t i = 0; i < 16; i++)
        to[i] = from[i];
}

void print_aes_state(aes_state state) {
    printf("{\n");
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            printf("%d, ", state[4*j + i]);
        }
        printf("\n");
    }
    printf("}\n");
}

void free_aes_state(aes_state state) {
    free(state);
}

void AddRoundKey(aes_state state, aes_state roundKey) {
    for (size_t i = 0; i < 16; i++)
    {
        state[i] ^= roundKey[i];
    }
    
}

void SubBytes(aes_state state) {
    /*
        use the 4-bit S-box to subsitute upper and lower nibble of a byte
    */
    uint8_t high, low;
    for (size_t i = 0; i < 16; i++)
    {
        high = (state[i] >> 4) & 0x0F;
        low  = state[i] & 0x0F;
        state[i] = (S_Box[high] << 4) | S_Box[low];
    }
}

void ShiftRows(aes_state state) {
    uint8_t helper = state[1];
    // second row:
    for (size_t i = 0; i < 3; i++)
        state[1 + 4*i] = state[1 + 4*i + 4];
    state[13] = helper;

    // third row
    helper = state[2];
    state[2] = state[10];
    state[10] = helper;    
    helper = state[6];
    state[6] = state[14];
    state[14] = helper;    

    // last row
    helper = state[15];
    for (size_t i = 0; i < 3; i++)
        state[15 - 4*i] = state[15 - 4*i - 4];
    state[3] = helper;
}

void ShiftRowsInv(aes_state state) {
    uint8_t helper = state[13];
    // second row:
    for (size_t i = 0; i < 3; i++)
        state[13 - 4*i] = state[13 - 4*i - 4];
    state[1] = helper;

    // third row
    helper = state[2];
    state[2] = state[10];
    state[10] = helper;    
    helper = state[6];
    state[6] = state[14];
    state[14] = helper;    

    // last row
    helper = state[3];
    for (size_t i = 0; i < 3; i++)
        state[3 + 4*i] = state[3 + 4*i + 4];
    state[15] = helper;
}

uint8_t Mult[16][16] = {
    {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
    {0, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15},
    {0, 2,  4,  6,  8,  10, 12, 14, 3,  1,  7,  5,  11, 9,  15, 13},
    {0, 3,  6,  5,  12, 15, 10, 9,  11, 8,  13, 14, 7,  4,  1,  2},
    {0, 4,  8,  12, 3,  7,  11, 15, 6,  2,  14, 10, 5,  1,  13, 9},
    {0, 5,  10, 15, 7,  2,  13, 8,  14, 11, 4,  1,  9,  12, 3,  6},
    {0, 6,  12, 10, 11, 13, 7,  1,  5,  3,  9,  15, 14, 8,  2,  4},
    {0, 7,  14, 9,  15, 8,  1,  6,  13, 10, 3,  4,  2,  5,  12, 11},
    {0, 8,  3,  11, 6,  14, 5,  13, 12, 4,  15, 7,  10, 2,  9,  1},
    {0, 9,  1,  8,  2,  11, 3,  10, 4,  13, 5,  12, 6,  15, 7,  14},
    {0, 10, 7,  13, 14, 4,  9,  3,  15, 5,  8,  2,  1,  11, 6,  12},
    {0, 11, 5,  14, 10, 1,  15, 4,  7,  12, 2,  9,  13, 6,  8,  3},
    {0, 12, 11, 7,  5,  9,  14, 2,  10, 6,  1,  13, 15, 3,  4,  8},
    {0, 13, 9,  4,  1,  12, 8,  5,  2,  15, 11, 6,  3,  14, 10, 7},
    {0, 14, 15, 1,  13, 3,  2,  12, 9,  7,  6,  8,  4,  10, 11, 5},
    {0, 15, 13, 2,  9,  6,  4,  11, 1,  14, 12, 3,  8,  7,  5,  10},
};

aes_state MixColumns(aes_state state) {
    aes_state target = create_aes_state();
    target[0]  = Mult[2][state[0]] ^ Mult[1][state[1]]  ^ Mult[1][state[2]]  ^ Mult[3][state[3]];
    target[1]  = Mult[3][state[0]] ^ Mult[2][state[1]]  ^ Mult[1][state[2]]  ^ Mult[1][state[3]];
    target[2]  = Mult[1][state[0]] ^ Mult[3][state[1]]  ^ Mult[2][state[2]]  ^ Mult[1][state[3]];
    target[3]  = Mult[1][state[0]] ^ Mult[1][state[1]]  ^ Mult[3][state[2]]  ^ Mult[2][state[3]];
    target[4]  = Mult[2][state[4]] ^ Mult[1][state[5]]  ^ Mult[1][state[6]]  ^ Mult[3][state[7]];
    target[5]  = Mult[3][state[4]] ^ Mult[2][state[5]]  ^ Mult[1][state[6]]  ^ Mult[1][state[7]];
    target[6]  = Mult[1][state[4]] ^ Mult[3][state[5]]  ^ Mult[2][state[6]]  ^ Mult[1][state[7]];
    target[7]  = Mult[1][state[4]] ^ Mult[1][state[5]]  ^ Mult[3][state[6]]  ^ Mult[2][state[7]];
    target[8]  = Mult[2][state[8]] ^ Mult[1][state[9]]  ^ Mult[1][state[10]] ^ Mult[3][state[11]];
    target[9]  = Mult[3][state[8]] ^ Mult[2][state[9]]  ^ Mult[1][state[10]] ^ Mult[1][state[11]];
    target[10] = Mult[1][state[8]] ^ Mult[3][state[9]]  ^ Mult[2][state[10]] ^ Mult[1][state[11]];
    target[11] = Mult[1][state[8]] ^ Mult[1][state[9]]  ^ Mult[3][state[10]] ^ Mult[2][state[11]];
    target[12] = Mult[2][state[12]]^ Mult[1][state[13]] ^ Mult[1][state[14]] ^ Mult[3][state[15]];
    target[13] = Mult[3][state[12]]^ Mult[2][state[13]] ^ Mult[1][state[14]] ^ Mult[1][state[15]];
    target[14] = Mult[1][state[12]]^ Mult[3][state[13]] ^ Mult[2][state[14]] ^ Mult[1][state[15]];
    target[15] = Mult[1][state[12]]^ Mult[1][state[13]] ^ Mult[3][state[14]] ^ Mult[2][state[15]];
    return target;
}

aes_state MixColumnsInv(aes_state state) {
    aes_state target = create_aes_state();
    target[0]  = Mult[14][state[0]]  ^ Mult[9][state[1]]   ^ Mult[13][state[2]]  ^ Mult[11][state[3]];
    target[1]  = Mult[11][state[0]]  ^ Mult[14][state[1]]  ^ Mult[9][state[2]]   ^ Mult[13][state[3]];
    target[2]  = Mult[13][state[0]]  ^ Mult[11][state[1]]  ^ Mult[14][state[2]]  ^ Mult[9][state[3]];
    target[3]  = Mult[9][state[0]]   ^ Mult[13][state[1]]  ^ Mult[11][state[2]]  ^ Mult[14][state[3]];
    target[4]  = Mult[14][state[4]]  ^ Mult[9][state[5]]   ^ Mult[13][state[6]]  ^ Mult[11][state[7]];
    target[5]  = Mult[11][state[4]]  ^ Mult[14][state[5]]  ^ Mult[9][state[6]]   ^ Mult[13][state[7]];
    target[6]  = Mult[13][state[4]]  ^ Mult[11][state[5]]  ^ Mult[14][state[6]]  ^ Mult[9][state[7]];
    target[7]  = Mult[9][state[4]]   ^ Mult[13][state[5]]  ^ Mult[11][state[6]]  ^ Mult[14][state[7]];
    target[8]  = Mult[14][state[8]]  ^ Mult[9][state[9]]   ^ Mult[13][state[10]] ^ Mult[11][state[11]];
    target[9]  = Mult[11][state[8]]  ^ Mult[14][state[9]]  ^ Mult[9][state[10]]  ^ Mult[13][state[11]];
    target[10] = Mult[13][state[8]]  ^ Mult[11][state[9]]  ^ Mult[14][state[10]] ^ Mult[9][state[11]];
    target[11] = Mult[9][state[8]]   ^ Mult[13][state[9]]  ^ Mult[11][state[10]] ^ Mult[14][state[11]];
    target[12] = Mult[14][state[12]] ^ Mult[9][state[13]]  ^ Mult[13][state[14]] ^ Mult[11][state[15]];
    target[13] = Mult[11][state[12]] ^ Mult[14][state[13]] ^ Mult[9][state[14]]  ^ Mult[13][state[15]];
    target[14] = Mult[13][state[12]] ^ Mult[11][state[13]] ^ Mult[14][state[14]] ^ Mult[9][state[15]];
    target[15] = Mult[9][state[12]]  ^ Mult[13][state[13]] ^ Mult[11][state[14]] ^ Mult[14][state[15]];
    return target;
}

void AES_encrypt(aes_state state, aes_state target, aes_state* keys, size_t round_count) {
    // r-round AES has r+1 keys
    // c = (ARKₖᵣ ◦ F) ◦ (ARKₖᵣ₋₁ ◦ F) ◦ ... ◦ (ARKₖ₁ ◦ F) ◦ ARKₖ₀(m)
    // F = MC ◦ SR ◦ SB

    copy_aes_state(state, target);
    AddRoundKey(target, keys[0]);

    for (size_t i = 1; i <= round_count; i++)
    {
        // SubBytes(target);
        // ShiftRows(target);
        target = MixColumns(target);
        // AddRoundKey(target, keys[i]);
        printf("round_%ld: ", i);
        print_aes_state(target);
    }
    
    return;
}

void AES_decrypt(aes_state state, aes_state target, aes_state* keys, size_t round_count) {
    copy_aes_state(state, target);

    for (int i = round_count; i > 0 ; i--)
    {
        // AddRoundKey(target, keys[i]);
        target = MixColumnsInv(target);
        // ShiftRowsInv(target);
        // SubBytes(target);  // is its own inverse
        printf("round_%d: ", i);
        print_aes_state(target);
    }

    AddRoundKey(target, keys[0]);
}