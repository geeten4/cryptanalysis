#ifndef GF_H
#define GF_H

#include <stdint.h>

// const uint16_t FIELD_MODULUS; // Rijndael polynomial 0b100011011 or x^8 + x^4 + x^3 + x + 1
typedef uint8_t gf2_4;

gf2_4 gf_add(gf2_4 a, gf2_4 b);
gf2_4 gf_mul(gf2_4 a, gf2_4 b);
gf2_4 rand_gf2_4();

#endif  // GF_H