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


int main() {
    fb_t fb_limit = 14;
    BasisList *fb = create_factor_basis(fb_limit);
    BasisList_print(fb);

    gf_t q = 14087, alpha = 5, beta = 5872;
    // gf_t q = 14087, alpha = 5;

    // gf_t lucky_guesses[] = {346, 171, 153, 442, 458, 461};
    int rows = 6*3, cols = 7;
    Matrix* A = matrix_create(rows, cols);

    gf_t rem = 2, s = 1;;
    for (size_t i = 0; i < rows; i++)
    {
        Vector *vec = vector_create(fb->size);
        rem = 2;
        while(rem > 1)
        {
            s += 1;
            vector_set_zeros(vec);
            rem = vector_express_in_fbasis(vec, fb, mod_pow(alpha, s, q));
        }
        matrix_set_row_vector(A, i, vec);
        matrix_set(A, i, cols - 1, s);
        // if (i != vector_multiply_in_fbasis(vec, fb)) {
        //     printf("%ld\n", i);
        // }
        vector_free(vec);
    }

    matrix_print(A);

    // q - 1 = 14086 = 2 * 7043
    // gf_t factors[] = {2, 7043};
    BasisList *factors = BasisList_create(2);
    factor(factors, q-1);
    printf("q-1 factors: ");
    BasisList_print(factors);
    int factor_count = factors->size;
    // return 0;

    Vector** solutions = malloc(sizeof(Vector*) * factor_count);
    for (size_t i = 0; i < factor_count; i++)
    {
        Matrix* B = matrix_copy(A);
        // matrix_print(B);
        gf_t m = BasisList_get(factors, i);
        matrix_gaussian_elimination(B, m);
        Vector* sol = vector_create(cols - 1);
        matrix_get_solution(B, sol, m);
        solutions[i] = sol;
        printf("Solution modulo %d: ", m);
        vector_print_with_zeros(sol);
        matrix_print(B);
        matrix_free(B);
    }

    Vector* combined_solution = vector_create(cols - 1);
    crt_combine_vectors(solutions, factors, factors->size, combined_solution);
    printf("combined solution mod q-1=%d: ", q - 1);
    vector_print_with_zeros(combined_solution);

    s = 1;
    rem = 2;
    Vector* s_vec = vector_create(cols - 1);
    while(rem > 1)
    {
        s += 1;
        vector_set_zeros(s_vec);
        rem = vector_express_in_fbasis(s_vec, fb, mod_mul(beta, mod_pow(alpha, s, q), q));
    }
    printf("s_vec: ");
    vector_print_with_zeros(s_vec);
    

    gf_t discrete_log = mod_sub(vector_dot_product_mod(combined_solution, s_vec, q-1), s, q-1);
    printf("Discrete logarithm: %d\n", discrete_log);

    // check
    printf("Checking %d = beta = alpha ^ discrete_log = %d ^ %d = %d\n", beta, alpha, discrete_log, mod_pow(alpha, discrete_log, q));

    // gf_t b = 101*15489, a = 101*15478;
    // gf_t* out = extended_euclidian_algorithm(a, b);
    // printf("a*x + b*y = %d*%d + %d*%d = %d", a, out[0], b, out[1], a*out[0] + b*out[1]);

    return 0;

    // matrix_set(A, 0, 0, 1);  matrix_set(A, 0, 1, 2);  matrix_set(A, 0, 2, 3); matrix_set(A, 0, 3, 8);
    // matrix_set(A, 1, 0, 3); matrix_set(A, 1, 1, 6); matrix_set(A, 1, 2, 2);  matrix_set(A, 1, 3, 11);
    // matrix_set(A, 2, 0, 0); matrix_set(A, 2, 1, 4);  matrix_set(A, 2, 2, 2);  matrix_set(A, 2, 3, 3);

    // // Row 0
    // matrix_set(A, 0, 0, 47);  matrix_set(A, 0, 1, 2);  matrix_set(A, 0, 2, 3);
    // matrix_set(A, 0, 3, 4);  matrix_set(A, 0, 4, 5);  matrix_set(A, 0, 5, 6);
    // matrix_set(A, 0, 6, 47);  // RHS

    // // Row 1
    // matrix_set(A, 1, 0, 0);  matrix_set(A, 1, 1, 1);  matrix_set(A, 1, 2, 2);
    // matrix_set(A, 1, 3, 3);  matrix_set(A, 1, 4, 4);  matrix_set(A, 1, 5, 5);
    // matrix_set(A, 1, 6, 6);  // RHS

    // // Row 2
    // matrix_set(A, 2, 0, 0);  matrix_set(A, 2, 1, 0);  matrix_set(A, 2, 2, 1);
    // matrix_set(A, 2, 3, 2);  matrix_set(A, 2, 4, 3);  matrix_set(A, 2, 5, 4);
    // matrix_set(A, 2, 6, 5);  // RHS

    // // Row 3
    // matrix_set(A, 3, 0, 0);  matrix_set(A, 3, 1, 0);  matrix_set(A, 3, 2, 0);
    // matrix_set(A, 3, 3, 1);  matrix_set(A, 3, 4, 2);  matrix_set(A, 3, 5, 3);
    // matrix_set(A, 3, 6, 4);  // RHS

    // // Row 4
    // matrix_set(A, 4, 0, 0);  matrix_set(A, 4, 1, 0);  matrix_set(A, 4, 2, 0);
    // matrix_set(A, 4, 3, 0);  matrix_set(A, 4, 4, 1);  matrix_set(A, 4, 5, 2);
    // matrix_set(A, 4, 6, 3);  // RHS

    // // Row 5
    // matrix_set(A, 5, 0, 0);  matrix_set(A, 5, 1, 0);  matrix_set(A, 5, 2, 0);
    // matrix_set(A, 5, 3, 0);  matrix_set(A, 5, 4, 0);  matrix_set(A, 5, 5, 1);
    // matrix_set(A, 5, 6, 2);  // RHS

    // printf("Before elimination:\n");
    // matrix_print(A);

    // gf_t p = 101;

    // matrix_gaussian_elimination(A, p);

    // printf("After Gaussian elimination:\n");
    // matrix_print(A);

    // Vector* sol = vector_create(6);

    // matrix_get_solution(A, sol, p);
    // printf("Solution: \n");
    // vector_print(sol);
    // vector_print_with_zeros(sol);

    // matrix_free(A);
    // return 0;

}