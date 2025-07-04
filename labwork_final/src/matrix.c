#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

Matrix* matrix_create(int rows, int cols) {
    Matrix* mat = malloc(sizeof(Matrix));
    mat->row_count = rows;
    mat->col_count = cols;
    mat->capacity = (rows > 0) ? rows : 4;
    mat->rows = malloc(mat->capacity * sizeof(Vector*));

    for (int i = 0; i < rows; ++i) {
        mat->rows[i] = vector_create(cols);
    }

    return mat;
}

Matrix* matrix_copy(Matrix* mat) {
    Matrix* out = matrix_create(mat->row_count, mat->col_count);
    for (size_t i = 0; i < mat->row_count; i++)
    {
        for (size_t j = 0; j < mat->col_count; j++)
        {
            matrix_set(out, i, j, matrix_get(mat, i, j));
        }
    }
    return out;
}

void matrix_set_row_vector(Matrix* mat, int row, const Vector* vec) {
    if (row >= mat->row_count) return;

    for (size_t i = 0; i < vec->length; i++) {
        matrix_set(mat, row, i, vector_get(vec, i));
    }
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
        vector_print_with_zeros(mat->rows[i]);
        // vector_print(mat->rows[i]);
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

    matrix_mod(mat, p);  // Ensure all entries are mod p

    for (int col = 0; col < cols && pivot_row < rows; ++col) {
        // --- Find invertible pivot ---
        int best_row = -1;
        gf_t inv = -1;

        for (int r = pivot_row; r < rows; ++r) {
            gf_t val = matrix_get(mat, r, col);
            inv = mod_inv(val, p);  // Use EEA-based mod_inv
            if (val != 0 && inv != (gf_t)-1) {
                best_row = r;
                break;
            }
        }

        // --- No invertible pivot: skip column ---
        if (best_row == -1) {
            continue;
        }

        // --- Swap row to pivot position ---
        if (best_row != pivot_row) {
            Vector* tmp = mat->rows[pivot_row];
            mat->rows[pivot_row] = mat->rows[best_row];
            mat->rows[best_row] = tmp;
        }

        // --- Normalize pivot row ---
        matrix_scale_row(mat, pivot_row, inv, p);

        // --- Eliminate entries below pivot ---
        for (int r = pivot_row + 1; r < rows; ++r) {
            gf_t below = matrix_get(mat, r, col);
            if (below != 0) {
                matrix_add_scaled_row(mat, r, pivot_row, mod_opp(below, p), p);
            }
        }

        pivot_row++;
    }
}

// Returns 1 if solution exists, and fills `solution` vector (size = col_count - 1)
// Returns 0 if no solution; exists
int matrix_get_solution(Matrix* mat, Vector* solution, gf_t p) {
    int rows = mat->row_count;
    int cols = mat->col_count; // includes RHS column
    int vars = cols - 1;

    // Initialize solution to zero
    for (int i = 0; i < vars; ++i) {
        vector_set(solution, i, 0);
    }

    // Check for inconsistency (e.g., 0x + 0y + 0z = non-zero)
    for (int r = 0; r < rows; ++r) {
        int all_zero = 1;
        for (int c = 0; c < vars; ++c) {
            if (matrix_get(mat, r, c) != 0) {
                all_zero = 0;
                break;
            }
        }
        if (all_zero && matrix_get(mat, r, vars) != 0) {
            // Inconsistent system
            return 0;
        }
    }

    // Perform back substitution
    for (int r = rows - 1; r >= 0; --r) {
        int pivot_col = -1;
        gf_t rhs = matrix_get(mat, r, vars); // Start with RHS

        // Find pivot in this row
        for (int c = 0; c < vars; ++c) {
            gf_t coeff = matrix_get(mat, r, c);
            if (coeff == 1) {
                pivot_col = c;
                break;
            }
        }

        if (pivot_col == -1) {
            // No pivot in this row (free variable or redundant row)
            continue;
        }

        // Subtract known variable values from RHS
        for (int c = pivot_col + 1; c < vars; ++c) {
            gf_t coeff = matrix_get(mat, r, c);
            if (coeff != 0) {
                rhs = mod_sub(rhs, mod_mul(coeff, vector_get(solution, c), p), p);
            }
        }

        // Since pivot is 1 in RREF, we directly assign:
        vector_set(solution, pivot_col, rhs % p);
    }

    return 1; // solution exists
}

void matrix_vector_mul(const Matrix* A, const Vector* v, Vector* result, gf_t p) {
    // Initialize result to zero
    for (int i = 0; i < A->row_count; ++i) {
        vector_set(result, i, 0);
    }

    for (int row = 0; row < A->row_count; ++row) {
        Vector* row_vec = A->rows[row];
        for (int i = 0; i < row_vec->size; ++i) {
            int col_index = row_vec->entries[i].index;
            gf_t A_ij = row_vec->entries[i].value;

            // Find v[col_index] in the sparse vector
            for (int j = 0; j < v->size; ++j) {
                if (v->entries[j].index == col_index) {
                    gf_t v_j = v->entries[j].value;
                    vector_set(result, row, (mod_add(vector_get(result, row), mod_mul(A_ij, v_j, p), p)));
                    break;
                }
            }
        }
    }
}

void matrix_mod(const Matrix* mat, gf_t p) {
    for (size_t i = 0; i < mat->row_count; i++)
        vector_mod(mat->rows[i], p);
}

Matrix* matrix_remove_zero_rows(Matrix* mat) {
    // assuming Matrix mat is in RREF, create new matrix with only non-zero rows
    size_t i = 0;
    while (true) {
        // vector_check_for_zeroes(mat->rows[i]);
        if (mat->rows[i]->size == 0) break;
        i++;
    };

    if (i == 0) {
        printf("Creating empty matrix, abort.");
        return 0;
    }

    mat->row_count=i;
    Matrix* out = matrix_copy(mat);
    free(mat);
    return out;
}
