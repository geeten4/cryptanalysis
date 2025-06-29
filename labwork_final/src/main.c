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


int main() {
    BasisList *fb = create_factor_basis(100);
    BasisList_print(fb);


    for (size_t i = 1; i < 100; i++)
    {
        Vector *vec = vector_create(fb->size);
        vector_express_in_fbasis(vec, fb, i);
        if (i != vector_multiply_in_fbasis(vec, fb)) {
            printf("%ld\n", i);
        }
        vector_free(vec);
    }

    // System:
    // 2x + y  - z =  8
    //-3x - y + 2z = -11
    //-2x + y + 2z = -3

    Matrix* A = matrix_create(3, 3);
    matrix_set(A, 0, 0, 2);  matrix_set(A, 0, 1, 1);  matrix_set(A, 0, 2, 1); matrix_set(A, 0, 3, 8);
    matrix_set(A, 1, 0, 3); matrix_set(A, 1, 1, 1); matrix_set(A, 1, 2, 2);  matrix_set(A, 1, 3, 11);
    matrix_set(A, 2, 0, 2); matrix_set(A, 2, 1, 1);  matrix_set(A, 2, 2, 2);  matrix_set(A, 2, 3, 3);

    printf("Before elimination:\n");
    matrix_print(A);

    matrix_gaussian_elimination(A, 13);

    printf("After Gaussian elimination:\n");
    matrix_print(A);

    matrix_free(A);
    return 0;

}