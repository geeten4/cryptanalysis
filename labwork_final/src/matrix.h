#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"
#include "gf.h"

typedef struct {
    Vector** rows;
    int row_count;
    int col_count;
    int capacity;
} Matrix;

Matrix* matrix_create(int initial_rows, int cols);
Matrix* matrix_copy(Matrix* mat);
void matrix_set_row_vector(Matrix* mat, int row, Vector* vec);
void matrix_set(Matrix* mat, int row, int col, gf_t value);
gf_t matrix_get(const Matrix* mat, int row, int col);
void matrix_scale_row(Matrix* mat, int row, gf_t scalar, gf_t p);
void matrix_add_scaled_row(Matrix* mat, int dest_row, int src_row, gf_t scalar, gf_t p);
void matrix_print(const Matrix* mat);
void matrix_free(Matrix* mat);
void matrix_gaussian_elimination(Matrix* mat, gf_t p);
int matrix_get_solution(Matrix* mat, Vector* solution, gf_t p);
void matrix_vector_mul(const Matrix* A, const Vector* v, Vector* result, gf_t p);

#endif // MATRIX_H