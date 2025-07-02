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
    for (int i = 0; i < vec->size; ++i) {
        printf("  [%d] = %d\n", vec->entries[i].index, vec->entries[i].value);
    }
}

void vector_print_with_zeros(const Vector* vec) {
    printf("[");
    for (int i = 0; i < vec->length; ++i) {
        int found = 0;
        for (int j = 0; j < vec->size; ++j) {
            if (vec->entries[j].index == i) {
                printf("%d", vec->entries[j].value);
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

gf_t* extended_euclidian_algorithm(gf_t a, gf_t b) {
    // computes x, y s.t. ax + by = gcd(a, b)
    gf_t x = 1, y = 0, z = 0, w = 1;
    gf_t c;
    while (b != 0) {
        int k = (int) a / b;
        a -= k*b;
        x -= k*z;
        y -= k*w;

        c = b;
        b = a;
        a = c;
        c = x;
        x = z;
        z = c;
        c = y;
        y = w;
        w = c;
    }

    gf_t* out = malloc(sizeof(gf_t) * 2);
    out[0] = x;
    out[1] = y;
    return out;
}

void crt_combine_vectors(Vector** vectors, BasisList* moduli, int n, Vector* v) {
    // Compute product m = a_1 * ... * a_n
    gf_t m = 1;
    for (int i = 0; i < n; ++i)
        m *= BasisList_get(moduli, i);

    int vector_dim = vectors[0]->length;
    Vector* helper = vector_create(vector_dim);

    // solve the first two
    for (size_t i = 0; i < vector_dim; i++)
    {
        if (n > 2)
            crt_combine_vectors(vectors, moduli, n - 1, helper);
        else
            helper = vectors[n - 2];

        // if (vector_get(helper, i) == 0 && vector_get(vectors[n - 1], i) == 0)
        //     continue;

        // printf("helper: ");
        // vector_print_with_zeros(helper);
        // printf("vectors[n-1]: ");
        // vector_print_with_zeros(vectors[n-1]);
        // printf("\n");

        gf_t n_1 = BasisList_get(moduli, n-1), n_2 = BasisList_get(moduli, n-2);
        gf_t* eea_coeffs = extended_euclidian_algorithm(n_1, n_2);
        gf_t m_1 = eea_coeffs[0], m_2 = eea_coeffs[1];
        gf_t a_1 = vector_get(vectors[n-1], i);
        gf_t a_2 = vector_get(helper, i);
        gf_t x = a_1 * m_2 * n_2 + a_2 * m_1 * n_1;
        x %= n_1 * n_2;
        if (x < 0) x += n_1 * n_2;

        vector_set(v, i, x);
    }
}
