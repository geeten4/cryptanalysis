#include "dl.h"

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
            printf("\nPartial solution for CRT mod %ld: \n", m);
            for (size_t i = 0; i < sol->length; i++)
                printf("log_%ld(%ld)=%ld (mod %ld)\n", alpha, BasisList_get(fb, i), vector_get(sol, i), m);
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
        printf("Found s=%ld s.t. beta * alpha ^ s = %ld * %ld ^ %ld = %ld (mod %ld) factors in the factor basis as:\n", s, beta, alpha, s, mod_mul(beta, mod_pow(alpha, s, q), q), q);
        vector_print_with_zeros(s_vec);
        printf("\n");
    }

    return mod_sub(vector_dot_product_mod(combined_solution, s_vec, q-1), s, q-1);
}

gf_t solve_dl(gf_t alpha, gf_t beta, gf_t q, int fb_limit, bool verbose) {
    BasisList *fb = create_factor_basis(fb_limit);

    if (verbose) {
        BasisList_print(fb);
        printf("q=%ld, alpha=%ld, beta=%ld\n", q, alpha, beta);
    }

    BasisList *factors = BasisList_create(2);
    factor(factors, q-1);
    factors = multiply_same_factors(factors);

    if (verbose) {
        printf("q-1 factors: ");
        BasisList_print(factors);
    }

    // sometimes, especially for prime power factors, the matrix A will not have a full rank modulo this
    // prime power factor, loop until A has full rank modulo all coprime factors
    // prevent infinite loop
    int MAX_TRY_COUNT = 100, try_count = 0;
    while (try_count < MAX_TRY_COUNT) {
        int rows = 3 * fb->size, cols = fb->size + 1;
        Matrix* A = matrix_create(rows, cols);

        matrix_add_random_vectors_in_fbasis(A, fb, rows, cols, alpha, q, verbose);

        Vector* combined_solution = solve_system_with_crt(A, factors, fb, cols, alpha, factors->size, verbose);
        
        if (verbose) {
            printf("\nCombined solution after CRT: \n");
            for (size_t i = 0; i < combined_solution->length; i++)
                printf("log_%ld(%ld)=%ld (mod %ld)\n", alpha, BasisList_get(fb, i), vector_get(combined_solution, i), q-1);
            printf("\n");
        }

        gf_t discrete_log = find_dl_from_combined_solution(combined_solution, fb, alpha, beta, q, cols, verbose);

        if (verbose) {
            printf("Discrete logarithm: %ld\n", discrete_log);
            printf("Checking %ld = beta = alpha ^ discrete_log = %ld ^ %ld = %ld\n", beta, alpha, discrete_log, mod_pow(alpha, discrete_log, q));
        }

        if (mod_pow(alpha, discrete_log, q) == beta)
            return discrete_log;
        else
            try_count++;

        matrix_free(A);
    }

    return 0;
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
