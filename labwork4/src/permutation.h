#ifndef PERMUTATION_H
#define PERMUTATION_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "gf.h"

gf2_11* initPermutation(FILE *file);
gf2_11 permutation(gf2_11 x);
gf2_11 permutation_inverse(gf2_11 x);

#endif  // PERMUTATION_H