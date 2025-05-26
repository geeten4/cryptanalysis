#ifndef AES_H
#define AES_H

#include <stdint.h>

typedef uint8_t* aes_state;


aes_state create_aes_state();
void print_aes_state(aes_state state);
void free_aes_state(aes_state state);
void SubBytes(aes_state state);
void ShiftRows(aes_state state);
void MixColumns(aes_state state, aes_state target);
void MixColumnsInv(aes_state state, aes_state target);

#endif  // AES_H