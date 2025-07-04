#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include "factor_basis.h"
#include "blist.h"
// #include "vector.h"
#include <math.h>
#include "gf.h"
#include "vector.h"
#include "matrix.h"
#include "utils.h"
#include "dl.h"

int main() {
    srand(time(NULL)); // Seed using current time

    fb_t fb_limit = 15;

    gf_t q = 14087, alpha = 5, beta = 5872;  // numbers from Table 4
    // gf_t q = 19079, alpha = 23, beta = 5872;  // 19078 is a product of two distinct primes, 23 is a generator
    // gf_t q = 19087, alpha = 37, beta = 5872;  // 19087 is a product of three distinct primes, 37 is a generator
    // gf_t q = 18061, alpha = 6, beta = 5872;  // 18060 = 2^2 * 3 * 5 * 7 * 43
    // gf_t q = 870871, alpha = 11, beta = 5872;  // 870870 = 2 * 3 * 5 * 7 * 11 * 13 * 29
    // gf_t q = 4987, alpha = 2, beta = 10;
    // q = 23250037;
    // 831867923631411555738439410809

    printf("is_generator(alpha, q): %d\n", is_generator(alpha, q));

    gf_t dl = solve_dl(alpha, beta, q, fb_limit, true);

    printf("dl: %d\n, mod_pow(alpha, discrete_log, q) == beta: %d\n", dl, mod_pow(alpha, dl, q) == beta);

    return 0;


}