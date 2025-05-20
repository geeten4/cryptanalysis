#include <stdint.h> 

#ifndef GF_H
#define GF_H

typedef uint16_t gf2_12;  // Field element: 12 bits max
#define FIELD_SIZE = 4096
#define FIELD_MODULUS 0x107b  // F_2^12 modulus x^12 + x^7 + x^6 + x^5 + x^3 + x + 1

#endif // GF_H