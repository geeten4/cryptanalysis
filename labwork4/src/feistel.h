#ifndef FEISTEL_H
#define FEISTEL_H

#include <stdint.h> 
#include "gf.h"

double_gf2_11 Feistel_encrypt(double_gf2_11 message, gf2_11 *keys, size_t roundCount);
double_gf2_11 Feistel_decrypt(double_gf2_11 cipher, gf2_11 *keys, size_t roundCount);

#endif  // FEISTEL_H