#ifndef GF_H
#define GF_H

#include <stdint.h> 

#define FIELD_SIZE = 4096
#define FIELD_MODULUS 0x107b  // F_2^12 modulus x^12 + x^7 + x^6 + x^5 + x^3 + x + 1

typedef uint16_t gf2_12;  // Field element: 12 bits max

gf2_12 gf_add(gf2_12 a, gf2_12 b);
gf2_12 gf_mul(gf2_12 a, gf2_12 b);
gf2_12 gf_pow(gf2_12 base, uint16_t exponent);
gf2_12 rand_gf2_12();

#endif // GF_H