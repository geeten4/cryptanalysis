#include <stdlib.h>
#include <stdio.h>

#include "aes.h"
#include "data.h"
#include "gf.h"


aes_state random_aes_state() {
    aes_state state = malloc(16 * sizeof(gf2_4));
    for (size_t j = 0; j < 16; j++)
    {
        state[j] = rand_gf2_4();
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
    return malloc(sizeof(gf2_4) * 16);
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
    for (size_t i = 0; i < 16; i++)
        state[i] = S_box[state[i]];
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

aes_state AES_encrypt(aes_state state, aes_state* keys, size_t round_count) {
    // r-round AES has r+1 keys
    // c = (ARKₖᵣ ◦ F) ◦ (ARKₖᵣ₋₁ ◦ F) ◦ ... ◦ (ARKₖ₁ ◦ F) ◦ ARKₖ₀(m)
    // F = MC ◦ SR ◦ SB

    aes_state target = create_aes_state();
    copy_aes_state(state, target);
    AddRoundKey(target, keys[0]);

    for (size_t i = 1; i <= round_count; i++)
    {
        SubBytes(target);
        ShiftRows(target);
        target = MixColumns(target);
        AddRoundKey(target, keys[i]);
    }
    
    return target;
}

aes_state AES_decrypt(aes_state state, aes_state* keys, size_t round_count) {
    aes_state target = create_aes_state();
    copy_aes_state(state, target);

    for (int i = round_count; i > 0 ; i--)
    {
        AddRoundKey(target, keys[i]);
        target = MixColumnsInv(target);
        ShiftRowsInv(target);
        SubBytes(target);
    }

    AddRoundKey(target, keys[0]);
    return target;
}
