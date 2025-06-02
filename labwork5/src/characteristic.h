#ifndef CHARACTERISTIC_H
#define CHARACTERISTIC_H

#include <stdbool.h>
#include "aes.h"

bool check_first_column_charac(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count);
bool check_all_zero_charac(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count);

#endif  // CHARACTERISTIC_H