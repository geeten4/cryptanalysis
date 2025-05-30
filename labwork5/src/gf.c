#include "gf.h"
#include <stdlib.h>

// const uint16_t FIELD_MODULUS = 0x011b; // Rijndael polynomial 0b100011011 or x^8 + x^4 + x^3 + x + 1

gf2_4 gf_add(gf2_4 a, gf2_4 b) {
    // add two 4-bit values
    return a ^ b;
}

gf2_4 gf_mul(gf2_4 a, gf2_4 b) {
    // multiply two 4-bit values
    uint8_t result = 0;

    for (int i = 0; i < 4; i++) {
        if ((b >> i) & 1)
            result ^= ((uint8_t)a << i);
    }

    // Modular reduction
    for (int i = 7; i >= 4; i--) {
        if (result & (1 << i)) {
            // result ^= (FIELD_MODULUS << (i - 8));
            result ^= (0x011b << (i - 4));
        }
    }

    return (gf2_4)(result & 0xF);  // Mask to 4 bits
}

gf2_4 rand_gf2_4() {
    return (gf2_4) (rand() & 0xF);
}
