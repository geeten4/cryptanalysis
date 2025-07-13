#ifndef CHARACTERISTIC_H
#define CHARACTERISTIC_H

#include <stdbool.h>
#include "aes.h"
#include "set.h"

bool check_diagonal_zero(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count);
bool check_first_column_zero(aes_state plaintext, aes_state key_guess, aes_state *keys, size_t round_count);
bool check_combined_charac(aes_state plaintext, aes_state first_key_guess, aes_state last_key_guess, aes_state* keys, size_t round_count, aes_state sum, aes_state helper, aes_state cipher);
set_arg_t four_gf2_4_to_set_arg_t(gf2_4 a_1, gf2_4 a_2, gf2_4 a_3, gf2_4 a_4);
set_arg_t eight_gf2_4_to_set_arg_t(gf2_4 a_1, gf2_4 a_2, gf2_4 a_3, gf2_4 a_4, gf2_4 a_5, gf2_4 a_6, gf2_4 a_7, gf2_4 a_8);

#endif  // CHARACTERISTIC_H