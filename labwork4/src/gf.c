#include <stdlib.h>

#include "gf.h"

const int FIELD_SIZE = 2048;

gf2_11 rand_gf2_11() {
    return (gf2_11) (rand() % (1 << 11));
}

double_gf2_11 rand_double_gf2_11() {
    return (double_gf2_11) (rand() % (1 << 22));
}
