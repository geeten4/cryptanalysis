#ifndef DL_H
#define DL_H

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include "factor_basis.h"
#include "blist.h"
#include "gf.h"
#include "vector.h"
#include "matrix.h"
#include "utils.h"

void matrix_add_random_vectors_in_fbasis(Matrix* A, BasisList *fb, int rows, int cols, fb_t alpha, gf_t q, bool verbose);
Vector* solve_system_with_crt(Matrix* A, BasisList *factors, BasisList *fb, int cols, gf_t alpha,  int factor_count, bool verbose);
gf_t find_dl_from_combined_solution(Vector* combined_solution, BasisList* fb, gf_t alpha, gf_t beta, gf_t q, int cols, bool verbose);
gf_t solve_dl(gf_t alpha, gf_t beta, gf_t q, int fb_limit, bool verbose);
bool is_generator(gf_t g, gf_t p);

#endif  // DL_H