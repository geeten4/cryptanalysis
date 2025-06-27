#ifndef CHARACTERISTIC_H
#define CHARACTERISTIC_H

#include <stdbool.h>
#include "aes.h"

bool check_diagonal_zero(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count);
bool check_first_column_zero(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count);
bool check_combined_charac(aes_state plaintext, aes_state first_key_guess, aes_state last_key_guess, aes_state* keys, size_t round_count);

#endif  // CHARACTERISTIC_H