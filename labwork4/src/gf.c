#include <stdlib.h>
#include <assert.h>

#include "gf.h"

const int FIELD_SIZE = 2048;  // 2^11
const int FIELD_SIZE_SQUARE = 4194304; // 2^22

gf2_11 rand_gf2_11() {
    return (gf2_11) (rand() % FIELD_SIZE);
}

double_gf2_11 rand_double_gf2_11() {
    return (double_gf2_11) (rand() % (1 << 22));
}

gf2_11 double_right(double_gf2_11 x) {
    assert(x < FIELD_SIZE_SQUARE);
    return x % FIELD_SIZE;
}

gf2_11 double_left(double_gf2_11 x) {
    assert(x < FIELD_SIZE_SQUARE);
    return x >> 11;
}

double_gf2_11 create_double(gf2_11 left, gf2_11 right) {
    assert(left < FIELD_SIZE);
    assert(right < FIELD_SIZE);
    return (left << 11) ^ right;
}
