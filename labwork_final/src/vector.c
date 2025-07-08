#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "vector.h"

Vector* vector_create(int length) {
    Vector* vec = malloc(sizeof(Vector));
    vec->capacity = INIT_CAPACITY;
    vec->entries = malloc(vec->capacity * sizeof(Entry));
    vec->size = 0;
    vec->length = length;
    return vec;
}

int vector_set(Vector* vec, int index, gf_t value) {
    if (index < 0 || index >= vec->length) return 0;

    for (int i = 0; i < vec->size; ++i) {
        if (vec->entries[i].index == index) {
            if (value == 0) {
                vec->entries[i] = vec->entries[vec->size - 1];
                vec->size--;
            } else {
                vec->entries[i].value = value;
            }
            return 1;
        }
    }

    if (value == 0) return 2;

    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->entries = realloc(vec->entries, vec->capacity * sizeof(Entry));
    }
    vec->entries[vec->size].index = index;
    vec->entries[vec->size].value = value;
    vec->size++;
    return 3;
}

gf_t vector_get(const Vector* vec, int index) {
    if (index < 0 || index >= vec->length) return 0;

    for (int i = 0; i < vec->size; ++i) {
        if (vec->entries[i].index == index) {
            return vec->entries[i].value;
        }
    }
    return 0;
}

void vector_increment(Vector* vec, int index) {
    gf_t current = vector_get(vec, index);
    vector_set(vec, index, current + 1);
}

void vector_set_zeros(Vector* vec) {
    // effectively sets all entries to zeros i.e. removes all contents
    free(vec->entries);
    vec->capacity = INIT_CAPACITY;
    vec->entries = malloc(vec->capacity * sizeof(Entry));
    vec->size = 0;
}

gf_t vector_dot_product_mod(Vector* v, Vector* w, gf_t p) {
    if(v->length != w->length) printf("%d = dim(v) != dim(2) = %d\n", v->length, w->length);

    gf_t prod = 0, v_i, w_i;

    for (size_t i = 0; i < v->length; i++)
    {
        v_i = vector_get(v, i), w_i = vector_get(w, i);
        if (v_i == 0 && w_i == 0) continue;

        prod = mod_add(prod, mod_mul(v_i, w_i, p), p);
    }
    
    return prod;
}

void vector_scale(Vector* vec, gf_t scalar, gf_t p) {
    for (int i = 0; i < vec->size; ++i) {
        vec->entries[i].value = mod_mul(vec->entries[i].value, scalar, p);
    }
}

void vector_add_scaled(Vector* vec, const Vector* other, gf_t scalar, gf_t p) {
    for (int i = 0; i < other->size; ++i) {
        int idx = other->entries[i].index;
        gf_t val = mod_mul(other->entries[i].value, scalar, p);
        gf_t current = vector_get(vec, idx);
        vector_set(vec, idx, mod_add(current, val, p));
    }
}

void vector_free(Vector* vec) {
    if (vec) {
        free(vec->entries);
        free(vec);
    }
}

void vector_print(const Vector* vec) {
    printf("(");
    for (int i = 0; i < vec->size; ++i) {
        printf("[%d] = %ld", vec->entries[i].index, vec->entries[i].value);
        if (i < vec->size - 1) printf(", ");
    }
    printf(")\n");
}

void vector_print_with_zeros(const Vector* vec) {
    printf("[");
    for (int i = 0; i < vec->length; ++i) {
        int found = 0;
        for (int j = 0; j < vec->size; ++j) {
            if (vec->entries[j].index == i) {
                printf("%ld", vec->entries[j].value);
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("0");
        }

        if (i < vec->length - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

gf_t vector_express_in_fbasis(Vector* vec, BasisList* fb, gf_t n) {
    // tries to express n in terms of the factor basis fb
    // returns 1 if successfull
    // otherwise returns > 1 remainder after all divisions
    assert(fb->size == vec->length);
    gf_t b, m = n;

    for (size_t i = 0; i < fb->size; i++)
    {
        b = BasisList_get(fb, i);
        while((m % b == 0) && (m > 1)) {
            m /= b;
            vector_increment(vec, i);
        }
    }

    return m;
}

gf_t vector_multiply_in_fbasis(Vector* vec, BasisList* fb) {
    // given a factor basis fb and a basis vector bv,
    // multiply the factor basis given exponents in bv
    // i.e. inverse to Vector_express
    assert(fb->size == vec->length);

    gf_t n = 1;
    for (size_t i = 0; i < vec->size; i++)
        n *= pow(BasisList_get(fb, vec->entries[i].index), vec->entries[i].value);
    
    return n;
}

static inline gf_t mod(gf_t a, gf_t m) {
    gf_t r = a % m;
    return r < 0 ? r + m : r;
}

gf_t* extended_euclidean_algorithm(gf_t a, gf_t b) {
    gf_t s0 = 1, s1 = 0;
    gf_t t0 = 0, t1 = 1;
    gf_t r0 = a, r1 = b;

    while (r1 != 0) {
        gf_t q = r0 / r1;

        gf_t r_temp = r0 - q * r1;
        r0 = r1;
        r1 = r_temp;

        gf_t s_temp = s0 - q * s1;
        s0 = s1;
        s1 = s_temp;

        gf_t t_temp = t0 - q * t1;
        t0 = t1;
        t1 = t_temp;
    }

    gf_t* result = malloc(3 * sizeof(gf_t));
    result[0] = r0;  // gcd(a, b)
    result[1] = s0;  // x such that a*x + b*y = gcd
    result[2] = t0;  // y such that a*x + b*y = gcd
    return result;
}

// gf_t* extended_euclidean_algorithm(gf_t a, gf_t b) {
//     gf_t s0 = 1, s1 = 0;
//     gf_t t0 = 0, t1 = 1;
//     gf_t r0 = a, r1 = b;

//     while (r1 != 0) {
//         gf_t q = r0 / r1;

//         gf_t r_temp = r0 - q * r1;
//         r0 = r1;
//         r1 = r_temp;

//         gf_t s_temp = s0 - q * s1;
//         s0 = s1;
//         s1 = s_temp;

//         gf_t t_temp = t0 - q * t1;
//         t0 = t1;
//         t1 = t_temp;
//     }

//     gf_t* result = malloc(3 * sizeof(gf_t));
//     result[0] = r0;  // gcd(a, b)
//     result[1] = s0;  // x such that a*x + b*y = gcd
//     result[2] = t0;  // y such that a*x + b*y = gcd
//     return result;
// }

// void crt_combine_vectors(Vector** vectors, BasisList* moduli, int n, Vector* v) {
//     int vector_dim = vectors[0]->length;

//     for (int i = 0; i < vector_dim; i++) {
//         gf_t x = 0;
//         gf_t M = 1;

//         // Compute the total modulus M = m1 * m2 * ... * mn
//         for (int j = 0; j < n; j++) {
//             M *= BasisList_get(moduli, j);
//         }

//         for (int j = 0; j < n; j++) {
//             gf_t mj = BasisList_get(moduli, j);
//             gf_t aj = vector_get(vectors[j], i);

//             gf_t Mj = M / mj;

//             // Extended Euclidean Algorithm to compute inverse of Mj mod mj
//             gf_t* eea_result = extended_euclidean_algorithm(Mj, mj);
//             gf_t inv = eea_result[1];  // inverse of Mj mod mj

//             // Make sure the inverse is positive
//             inv = (inv % mj + mj) % mj;

//             free(eea_result);

//             x += aj * Mj * inv;
//         }

//         x %= M;
//         if (x < 0) x += M;

//         vector_set(v, i, x);
//     }
// }

// void crt_combine_vectors(Vector** vectors, BasisList* moduli, int n, Vector* result) {
//     int dim = vectors[0]->length;

//     for (int i = 0; i < dim; i++) {
//         gf_t x = vector_get(vectors[0], i);  // Initial remainder
//         gf_t M = BasisList_get(moduli, 0);   // Initial modulus

//         for (int k = 1; k < n; k++) {
//             gf_t mk = BasisList_get(moduli, k);
//             gf_t ak = vector_get(vectors[k], i);

//             gf_t x_mod_mk = x % mk;
//             if (x_mod_mk < 0) x_mod_mk += mk;

//             gf_t delta = (ak - x_mod_mk) % mk;
//             if (delta < 0) delta += mk;

//             // Compute M^{-1} mod mk
//             gf_t* eea = extended_euclidean_algorithm(M, mk);
//             gf_t M_inv = eea[1];  // M^{-1} mod mk
//             free(eea);
//             M_inv = (M_inv % mk + mk) % mk;

//             gf_t correction = (delta * M_inv) % mk;
//             x = x + correction * M;
//             M *= mk;
//         }

//         x = x % M;
//         if (x < 0) x += M;

//         vector_set(result, i, x);
//     }
// }

void crt_combine_vectors(Vector** vectors, BasisList* moduli, int n, Vector* result) {
    int dim = vectors[0]->length;

    for (int i = 0; i < dim; i++) {
        gf_t x = mod(vector_get(vectors[0], i), BasisList_get(moduli, 0));  // initial remainder
        gf_t M = BasisList_get(moduli, 0);  // initial modulus

        for (int k = 1; k < n; k++) {
            gf_t mk = BasisList_get(moduli, k);
            gf_t ak = mod(vector_get(vectors[k], i), mk);

            gf_t x_mod_mk = mod(x, mk);

            gf_t delta = mod(ak - x_mod_mk, mk);

            // Compute gcd and modular inverse of M mod mk
            gf_t* eea = extended_euclidean_algorithm(M, mk);
            gf_t gcd = eea[0];
            gf_t M_inv = eea[1];
            free(eea);

            if (gcd != 1) {
                // moduli not coprime; CRT invalid
                fprintf(stderr, "Error: moduli %ld and %ld are not coprime (gcd = %ld)\n", M, mk, gcd);
                // Handle error appropriately here (e.g., abort or skip)
                return;
            }

            M_inv = mod(M_inv, mk);  // positive inverse mod mk

            gf_t correction = mod(delta * M_inv, mk);

            // Carefully multiply to avoid overflow:
            // If gf_t is 64-bit, use 128-bit or check bounds before multiply
            x = x + correction * M;

            // Update combined modulus
            M = M * mk;
        }

        x = mod(x, M);
        vector_set(result, i, x);
    }
}

void vector_mod(Vector* vec, gf_t p) {
    for (size_t i = 0; i < vec->length; i++)
        vector_set(vec, i, vector_get(vec, i) % p);
}

// bool vector_is_zero(Vector* vec) {
//     if (vec->size == 0)
// }

// bool vector_has_zero_entries(Vector* vec) {
//     // due to errors, vector can have zero entries in vec->entries
//     // delete the zeroes
    
//     // TODO: find out why and when this happens
//     for (size_t i = 0; i < vec->size; i++)
//     {
//         if (vec->entries[i].value == 0) {
//             // printf("ZERO VALUES!!!!\n");
//             return true;
//             vector_set(vec, i, 0);
//         }
//     }
//     return false;
// }