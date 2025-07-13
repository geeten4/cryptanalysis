#ifndef AES_H
#define AES_H

#include <stdint.h>
#include "gf.h"

typedef gf2_4* aes_state;


void AES_encrypt(aes_state cipher, aes_state message, aes_state* keys, size_t round_count);
void AES_decrypt(aes_state cipher, aes_state message, aes_state* keys, size_t round_count);
aes_state random_aes_state();
aes_state *generate_keys(size_t key_count);
aes_state create_aes_state();
void copy_aes_state(aes_state from, aes_state to);
void print_aes_state(aes_state state);
void free_aes_state(aes_state state);
void add_to_state(aes_state add_to, aes_state to_be_added);
void AddRoundKey(aes_state state, aes_state roundKey);
void SubBytes(aes_state state);
void ShiftRows(aes_state state);
void ShiftRowsInv(aes_state state);
void MixColumns(aes_state state);
void MixColumnsInv(aes_state state);

#endif  // AES_H