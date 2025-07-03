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

void matrix_add_random_vectors_in_fbasis(Matrix* A, BasisList *fb, int rows, int cols, fb_t alpha, gf_t q, bool verbose) {
    gf_t rem = 2, s;
    for (size_t i = 0; i < rows; i++)
    {
        Vector *vec = vector_create(fb->size);
        rem = 2;
        while(rem > 1)
        {
            s = rand() % (q-1);
            vector_set_zeros(vec);
            rem = vector_express_in_fbasis(vec, fb, mod_pow(alpha, s, q));
        }
        matrix_set_row_vector(A, i, vec);
        matrix_set(A, i, cols - 1, s);
        vector_free(vec);
    }
}

Vector* solve_system_with_crt(Matrix* A, BasisList *factors, BasisList *fb, int cols, gf_t alpha,  int factor_count, bool verbose) {
    Vector** solutions = malloc(sizeof(Vector*) * factor_count);
    for (size_t i = 0; i < factor_count; i++)
    {
        Matrix* B = matrix_copy(A);
        gf_t m = BasisList_get(factors, i);
        Vector* sol = vector_create(cols - 1);

        matrix_gaussian_elimination(B, m);
        matrix_get_solution(B, sol, m);
        solutions[i] = sol;

        if (verbose) {
            printf("\nPartial solution for CRT mod %d: \n", m);
            for (size_t i = 0; i < sol->length; i++)
                printf("log_%d(%d)=%d (mod %d)\n", alpha, BasisList_get(fb, i), vector_get(sol, i), m);
            printf("\n");
        }

        matrix_free(B);
    }

    Vector* combined_solution = vector_create(cols - 1);
    crt_combine_vectors(solutions, factors, factors->size, combined_solution);
    return combined_solution;
}

gf_t find_dl_from_combined_solution(Vector* combined_solution, BasisList* fb, gf_t alpha, gf_t beta, gf_t q, int cols, bool verbose) {
    gf_t s = 1, rem = 2;
    Vector* s_vec = vector_create(cols - 1);

    while(rem > 1) {
        s += 1;
        vector_set_zeros(s_vec);
        rem = vector_express_in_fbasis(s_vec, fb, mod_mul(beta, mod_pow(alpha, s, q), q));
    }

    if (verbose) {
        printf("Found s=%d s.t. beta * alpha ^ s = %d * %d ^ %d = %d (mod %d) factors in the factor basis as:\n", s, beta, alpha, s, mod_mul(beta, mod_pow(alpha, s, q), q), q);
        vector_print_with_zeros(s_vec);
        printf("\n");
    }

    return mod_sub(vector_dot_product_mod(combined_solution, s_vec, q-1), s, q-1);
}

gf_t solve_dl(gf_t alpha, gf_t beta, gf_t q, int fb_limit, bool verbose) {
    BasisList *fb = create_factor_basis(fb_limit);

    if (verbose) {
        BasisList_print(fb);
        printf("q=%d, alpha=%d, beta=%d\n", q, alpha, beta);
    }

    int rows = 40, cols = 7;
    Matrix* A = matrix_create(rows, cols);

    matrix_add_random_vectors_in_fbasis(A, fb, rows, cols, alpha, q, verbose);

    BasisList *factors = BasisList_create(2);
    factor(factors, q-1);

    if (verbose) {
        printf("q-1 factors: ");
        BasisList_print(factors);
    }

    Vector* combined_solution = solve_system_with_crt(A, factors, fb, cols, alpha, factors->size, verbose);
    
    if (verbose) {
        printf("\nCombined solution after CRT: \n");
        for (size_t i = 0; i < combined_solution->length; i++)
            printf("log_%d(%d)=%d (mod %d)\n", alpha, BasisList_get(fb, i), vector_get(combined_solution, i), q-1);
        printf("\n");
    }

    gf_t discrete_log = find_dl_from_combined_solution(combined_solution, fb, alpha, beta, q, cols, verbose);

    if (verbose) {
        printf("Discrete logarithm: %d\n", discrete_log);
        printf("Checking %d = beta = alpha ^ discrete_log = %d ^ %d = %d\n", beta, alpha, discrete_log, mod_pow(alpha, discrete_log, q));
    }

    return discrete_log;
}

// Check if g is a generator modulo p
bool is_generator(gf_t g, gf_t p) {
    BasisList* factors = BasisList_create(10);
    factor(factors, p - 1);

    for (int i = 0; i < factors->size; i++) {
        gf_t factor = BasisList_get(factors, i);
        gf_t power = (p - 1) / factor;
        if (mod_pow(g, power, p) == 1) {
            return false;
        }
    }
    return true;
}

int main() {
    srand(time(NULL)); // Seed using current time

    fb_t fb_limit = 14;
    

    // gf_t q = 14087, alpha = 5, beta = 5872;  // numbers from Table 4
    // gf_t q = 19079, alpha = 23, beta = 5872;  // 19078 is a product of two distinct primes, 23 is a generator
    // gf_t q = 19087, alpha = 37, beta = 5872;  // 19087 is a product of three distinct primes, 37 is a generator
    // gf_t q = 18061, alpha = 6, beta = 5872;  // 18060 = 2^2 * 3 * 5 * 7 * 43
    gf_t q = 870871, alpha = 11, beta = 5872;  // 870870 = 2 * 3 * 5 * 7 * 11 * 13 * 29
    // 831867923631411555738439410809

    printf("is_generator(alpha, q): %d\n", is_generator(alpha, q));

    gf_t dl = solve_dl(alpha, beta, q, fb_limit, false);

    printf("dl: %d\n, mod_pow(alpha, discrete_log, q) == beta: %d\n", dl, mod_pow(alpha, dl, q) == beta);

    return 0;


}