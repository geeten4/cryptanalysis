#include "gf.h"

gf_t mod_mul(gf_t a, gf_t b, gf_t p) {
    return (gf_t)((1LL * a * b) % p);
}

gf_t mod_div(gf_t a, gf_t b, gf_t p) {
    return mod_mul(a, mod_inv(b, p), p);
}

gf_t mod_add(gf_t a, gf_t b, gf_t p) {
    gf_t res = a + b;
    if (res >= p) res -= p;
    return res;
}

gf_t mod_sub(gf_t a, gf_t b, gf_t p) {
    gf_t res = a - b;
    if (res < 0) res += p;
    return res;
}

gf_t mod_inv(gf_t a, gf_t p) {
    return mod_pow(a, p - 2, p);
}

int mod_pow(gf_t base, gf_t exp, gf_t p) {
    gf_t result = 1;
    base %= p;
    while (exp > 0) {
        if (exp & 1) result = mod_mul(result, base, p);
        base = mod_mul(base, base, p);
        exp >>= 1;
    }
    return result;
}