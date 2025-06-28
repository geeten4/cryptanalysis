#ifndef GF_H
#define GF_H

#include "factor_basis.h"

typedef int32_t gf_t;

gf_t mod_mul(gf_t a, gf_t b, gf_t p);
gf_t mod_div(gf_t a, gf_t b, gf_t p);
gf_t mod_add(gf_t a, gf_t b, gf_t p);
gf_t mod_sub(gf_t a, gf_t b, gf_t p);
int mod_pow(gf_t base, gf_t exp, gf_t p);
gf_t mod_inv(gf_t a, gf_t p);


#endif  // GF_H