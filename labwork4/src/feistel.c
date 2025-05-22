#include "feistel.h"
#include "gf.h"
#include "permutation.h"


gf2_11 roundFunction(gf2_11 x, gf2_11 key) {
    return permutation(x ^ key);
}

double_gf2_11 Feistel(double_gf2_11 message, gf2_11 *keys, size_t roundCount) {
    
}