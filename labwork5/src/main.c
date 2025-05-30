#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "aes.h"
#include <time.h>
#include "gf.h"
#include "data.h"


void firstExercise() {
    /*
        aes_state as 4x4 8-bit value (128 bits total) too complex, 
        implemented 4x4 4-bit value (64 bits total)
    */
    size_t round_count = 4;
    size_t key_count = round_count + 1;
    aes_state *keys = generate_keys(key_count);
    // print keys
    for (size_t i = 0; i < key_count; i++)
    {
        printf("k_%ld: ", i);
        print_aes_state(keys[i]);
    }

    uint64_t key_nibble_limit = 1 << 16;
    for (uint64_t key_nibbles = 0; key_nibbles < key_nibble_limit; key_nibbles++)
    {
        aes_state plaintext = random_aes_state(), sum = create_aes_state(), random_state = create_aes_state();
        random_state[0] = key_nibbles & 0xF;
        random_state[1] = (key_nibbles >> 4) & 0xF;
        random_state[2] = (key_nibbles >> 8) & 0xF;
        random_state[3] = (key_nibbles >> 12) & 0xF;

        for (size_t i = 0; i < 16; i++)
        {
            // create the structure
            plaintext[0] = i;

            // encrypt
            aes_state cipher = AES_encrypt(plaintext, keys, round_count);

            // rework last round
            // AddRoundKey(cipher, keys[4]);
            AddRoundKey(cipher, random_state);
            cipher = MixColumnsInv(cipher);
            ShiftRowsInv(cipher);
            SubBytes(cipher);

            // add to sum
            AddRoundKey(sum, cipher);
        }

        if (sum[0] == 0 && sum[5] == 0 && sum[10] == 0 && sum[15] == 0) {
            
            printf("possible key: \n");
            printf("key_4[0]: %d, ", random_state[0]);
            printf("key_4[1]: %d, ", random_state[1]);
            printf("key_4[2]: %d, ", random_state[2]);
            printf("key_4[3]: %d\n", random_state[3]);
        }
    }
    



    // for (size_t i = 0; i < 4294967296; i++)
    // {
    //     // printf("")
    // }
    
}

int main() {
    srand(time(NULL));   // Initialization, should only be called once.
    
    firstExercise();
    
    // size_t round_count = 1;
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
