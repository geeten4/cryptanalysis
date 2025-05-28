#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "aes.h"
#include <time.h>
#include "gf.h"


void firstExercise() {
    size_t round_count = 4;
    size_t key_count = round_count + 1;
    aes_state *keys = generate_keys(key_count);
    // print keys
    // for (size_t i = 0; i < key_count; i++)
    // {
    //     printf("k_%ld: ", i);
    //     print_aes_state(keys[i]);
    // }

    aes_state plaintext = random_aes_state(), cipher = create_aes_state(), sum = create_aes_state();
    aes_state random_state = random_aes_state();
    for (size_t i = 4; i < 15; i++)
    {
        random_state[i] = 0 ;
    }
    

    for (size_t i = 0; i < 256; i++)
    {
        // create the structure
        plaintext[0] = i;
        printf("plaintext: ");
        print_aes_state(plaintext);

        // encrypt
        AES_encrypt(plaintext, cipher, keys, round_count);
        printf("cipher: ");
        print_aes_state(cipher);

        // rework last round
        AddRoundKey(cipher, keys[4]);
        MixColumnsInv(cipher);
        ShiftRowsInv(cipher);
        SubBytes(cipher);

        // add to sum
        AddRoundKey(sum, cipher);

        printf("sum: ");
        print_aes_state(sum);

    }


    // for (size_t i = 0; i < 4294967296; i++)
    // {
    //     // printf("")
    // }
    
}

int main() {
    srand(time(NULL));   // Initialization, should only be called once.
    
    printf("%d", gf_mul(3, 2));
    // firstExercise();
    
    
    // size_t round_count = 1;
    // size_t key_count = round_count + 1;
    // aes_state *keys = generate_keys(key_count);

    // aes_state state = create_aes_state(), target = create_aes_state(), target_decrypted = create_aes_state();
    // state[0] = 1;
    // state[5] = 1;
    // state[10] = 1;
    // state[15] = 1;

    // print_aes_state(keys[0]);
    // print_aes_state(state);
    
    // AddRoundKey(state, keys[0]);
    // print_aes_state(state);

    // state = MixColumnsInv(state);
    // AddRoundKey(state, keys[0]);
    // print_aes_state(state);
    // AES_encrypt(state, target, keys, round_count);
    // AES_decrypt(target, target_decrypted, keys, round_count);
    // print_aes_state(target_decrypted);

}
