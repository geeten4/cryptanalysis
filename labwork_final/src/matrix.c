#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

Matrix* matrix_create(int initial_rows, int cols) {
    Matrix* mat = malloc(sizeof(Matrix));
    mat->row_count = initial_rows;
    mat->col_count = cols;
    mat->capacity = (initial_rows > 0) ? initial_rows : 4;
    mat->rows = malloc(mat->capacity * sizeof(Vector*));

    for (int i = 0; i < initial_rows; ++i) {
        mat->rows[i] = vector_create(cols);
    }

    return mat;
}

void matrix_ensure_row(Matrix* mat, int row) {
    if (row < mat->row_count) return;

    if (row >= mat->capacity) {
        while (row >= mat->capacity) mat->capacity *= 2;
        mat->rows = realloc(mat->rows, mat->capacity * sizeof(Vector*));
    }

    for (int i = mat->row_count; i <= row; ++i) {
        mat->rows[i] = vector_create(mat->col_count);
    }

    mat->row_count = row + 1;
}

void matrix_set(Matrix* mat, int row, int col, gf_t value) {
    matrix_ensure_row(mat, row);
    vector_set(mat->rows[row], col, value);
}

gf_t matrix_get(const Matrix* mat, int row, int col) {
    if (row >= mat->row_count) return 0.0;
    return vector_get(mat->rows[row], col);
}

void matrix_scale_row(Matrix* mat, int row, gf_t scalar, gf_t p) {
    if (row < mat->row_count) {
        vector_scale(mat->rows[row], scalar, p);
    }
}

void matrix_add_scaled_row(Matrix* mat, int dest_row, int src_row, gf_t scalar, gf_t p) {
    if (src_row >= mat->row_count) return;
    matrix_ensure_row(mat, dest_row);
    vector_add_scaled(mat->rows[dest_row], mat->rows[src_row], scalar, p);
}

void matrix_print(const Matrix* mat) {
    printf("Matrix (%d x %d):\n", mat->row_count, mat->col_count);
    for (int i = 0; i < mat->row_count; ++i) {
        printf("Row %d:\n", i);
        vector_print(mat->rows[i]);
    }
}

void matrix_free(Matrix* mat) {
    if (!mat) return;
    for (int i = 0; i < mat->row_count; ++i) {
        vector_free(mat->rows[i]);
    }
    free(mat->rows);
    free(mat);
}

void matrix_gaussian_elimination(Matrix* mat, gf_t p) {
    int rows = mat->row_count;
    int cols = mat->col_count;
    int pivot_row = 0;

    for (int col = 0; col < cols && pivot_row < rows; ++col) {
        // find pivot
        int best_row = -1;
        for (int r = pivot_row; r < rows; ++r) {
            gf_t val = matrix_get(mat, r, col);
            if (val != 0.0) {
                best_row = r;
                break;
            }
        }

        if (best_row == -1) {
            continue; // no pivot found
        }

        // swap row to the top
        if (best_row != pivot_row) {
            Vector* tmp = mat->rows[pivot_row];
            mat->rows[pivot_row] = mat->rows[best_row];
            mat->rows[best_row] = tmp;
        }

        // normalize pivot row
        gf_t pivot_val = matrix_get(mat, pivot_row, col);
        if (pivot_val != 1) {
            matrix_scale_row(mat, pivot_row, mod_inv(pivot_val, p), p);
        }

        // eliminate below
        for (int r = pivot_row + 1; r < rows; ++r) {
            gf_t below = matrix_get(mat, r, col);
            if (below != 0) {
                matrix_add_scaled_row(mat, r, pivot_row, mod_opp(below, p), p);
            }
        }

        pivot_row++;
    }
}