// #include <stdio.h>
// #include <stdlib.h>

// #define MOD 7  // finite field modulus (prime)
// #define N 3    // system size

// // Sparse row representation
// typedef struct {
//     int col[3];    // column indices
//     int val[3];    // values
//     int count;     // number of nonzero entries
// } SparseRow;

// // Modular inverse using Fermat's little theorem
// int modinv(int a, int p) {
//     int res = 1;
//     int e = p - 2;
//     while (e > 0) {
//         if (e % 2) res = (1LL * res * a) % p;
//         a = (1LL * a * a) % p;
//         e /= 2;
//     }
//     return res;
// }

// // Sparse modular Gaussian elimination
// void sparse_solve(SparseRow A[N], int b[N], int x[N]) {
//     for (int i = 0; i < N; i++) {
//         // Find pivot (assume A[i].col[0] == i)
//         int pivot_val = 0;
//         for (int k = 0; k < A[i].count; k++) {
//             if (A[i].col[k] == i) {
//                 pivot_val = A[i].val[k];
//                 break;
//             }
//         }
//         if (pivot_val == 0) {
//             printf("Singular matrix at row %d\n", i);
//             exit(1);
//         }

//         int inv = modinv(pivot_val, MOD);

//         // Normalize pivot row
//         for (int k = 0; k < A[i].count; k++)
//             A[i].val[k] = (A[i].val[k] * inv) % MOD;

//         b[i] = (b[i] * inv) % MOD;

//         // Eliminate below
//         for (int j = i + 1; j < N; j++) {
//             int coeff = 0;
//             int idx = -1;

//             // Find A[j][i]
//             for (int k = 0; k < A[j].count; k++) {
//                 if (A[j].col[k] == i) {
//                     coeff = A[j].val[k];
//                     idx = k;
//                     break;
//                 }
//             }

//             if (coeff == 0) continue;

//             // Subtract coeff * row i from row j
//             for (int k = 0; k < A[i].count; k++) {
//                 int col = A[i].col[k];
//                 int val = (1LL * coeff * A[i].val[k]) % MOD;

//                 // Find and subtract from A[j][col]
//                 int found = 0;
//                 for (int m = 0; m < A[j].count; m++) {
//                     if (A[j].col[m] == col) {
//                         A[j].val[m] = (A[j].val[m] - val + MOD) % MOD;
//                         found = 1;
//                         break;
//                     }
//                 }

//                 // If not found and value is non-zero, add new entry
//                 if (!found && val != 0) {
//                     A[j].col[A[j].count] = col;
//                     A[j].val[A[j].count] = (MOD - val) % MOD;
//                     A[j].count++;
//                 }
//             }

//             // Eliminate b[j]
//             b[j] = (b[j] - (1LL * coeff * b[i]) % MOD + MOD) % MOD;

//             // Zero out A[j][i]
//             if (idx != -1) {
//                 for (int m = idx; m < A[j].count - 1; m++) {
//                     A[j].col[m] = A[j].col[m + 1];
//                     A[j].val[m] = A[j].val[m + 1];
//                 }
//                 A[j].count--;
//             }
//         }
//     }

//     // Back substitution
//     for (int i = N - 1; i >= 0; i--) {
//         x[i] = b[i];
//         for (int k = 0; k < A[i].count; k++) {
//             if (A[i].col[k] > i) {
//                 x[i] = (x[i] - (1LL * A[i].val[k] * x[A[i].col[k]]) % MOD + MOD) % MOD;
//             }
//         }
//     }
// }

// int main() {
//     SparseRow A[N] = {0};
//     int b[N] = {1, 4, 2};
//     int x[N];

//     // Example sparse matrix:
//     // [2 0 1]
//     // [0 5 0]
//     // [3 0 6]

//     A[0].col[0] = 0; A[0].val[0] = 2;
//     A[0].col[1] = 2; A[0].val[1] = 1; A[0].count = 2;

//     A[1].col[0] = 1; A[1].val[0] = 5; A[1].count = 1;

//     A[2].col[0] = 0; A[2].val[0] = 3;
//     A[2].col[1] = 2; A[2].val[1] = 6; A[2].count = 2;

//     sparse_solve(A, b, x);

//     printf("Solution x (mod %d):\n", MOD);
//     for (int i = 0; i < N; i++) {
//         printf("x[%d] = %d\n", i, x[i]);
//     }

//     return 0;
// }