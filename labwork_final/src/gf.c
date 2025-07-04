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

// gf_t mod_inv(gf_t a, gf_t p) {
//     return mod_pow(a, p - 2, p);
// }

gf_t mod_inv(gf_t a, gf_t m) {
    gf_t t = 0, new_t = 1;
    gf_t r = m, new_r = a;

    while (new_r != 0) {
        gf_t quotient = r / new_r;

        gf_t temp_t = t;
        t = new_t;
        new_t = temp_t - quotient * new_t;

        gf_t temp_r = r;
        r = new_r;
        new_r = temp_r - quotient * new_r;
    }

    if (r > 1) {
        // a is not invertible mod m
        return -1;
    }

    if (t < 0) t += m;

    return t;
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

gf_t mod_opp(gf_t a, gf_t p) {
    return p - (a % p);
}