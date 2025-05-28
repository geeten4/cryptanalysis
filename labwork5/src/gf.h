#ifndef GF_H
#define GF_H

#include <stdint.h>

const uint16_t FIELD_MODULUS = 0x011b; // Rijndael polynomial 0b100011011 or x^8 + x^4 + x^3 + x + 1
typedef uint8_t gf2_8;

gf2_8 gf_add(gf2_8 a, gf2_8 b);
gf2_8 gf_mul(gf2_8 a, gf2_8 b);

#endif  // GF_H