#ifndef UTILS_H
#define UTILS_H

#include "gf.h"
#include "blist.h"

void factor(BasisList* bl, gf_t n);
BasisList* multiply_same_factors(BasisList* bl);

#endif