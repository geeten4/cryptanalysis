#include "gf.h"

gf2_8 gf_add(gf2_8 a, gf2_8 b) {
    return a ^ b;
}

gf2_8 gf_mul(gf2_8 a, gf2_8 b) {
    uint16_t result = 0;

    for (int i = 0; i < 8; i++) {
        if ((b >> i) & 1)
            result ^= ((uint16_t)a << i);
    }

    // Modular reduction
    for (int i = 15; i >= 8; i--) {
        if (result & (1 << i)) {
            result ^= (FIELD_MODULUS << (i - 8));
        }
    }

    return (gf2_8)(result & 0xFF);  // Mask to 8 bits
}