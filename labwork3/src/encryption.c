#include "encryption.h"
#include "gf.h"
#include "permutation.h"

gf2_12 encrypt(gf2_12 x, gf2_12 key) {
    // F(x + key) + key
    return gf_add(permutation_F(gf_add(x, key)), key);
}

gf2_12 decrypt(gf2_12 x, gf2_12 key) {
    // F^-1(x + key) + key
    return gf_add(permutation_F_inverse(gf_add(x, key)), key);
}

gf2_12 EDE_2(gf2_12 m, gf2_12 k_1, gf2_12 k_2) {
    return encrypt(decrypt(encrypt(m, k_1), k_2), k_1);
}

gf2_12 EDE_2_decrypt(gf2_12 m, gf2_12 k_1, gf2_12 k_2) {
    return decrypt(encrypt(decrypt(m, k_1), k_2), k_1);
}
