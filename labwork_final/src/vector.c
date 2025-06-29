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

void vector_set(Vector* vec, int index, gf_t value) {
    if (index < 0 || index >= vec->length) return;

    for (int i = 0; i < vec->size; ++i) {
        if (vec->entries[i].index == index) {
            if (value == 0) {
                vec->entries[i] = vec->entries[vec->size - 1];
                vec->size--;
            } else {
                vec->entries[i].value = value;
            }
            return;
        }
    }

    if (value == 0) return;

    if (vec->size == vec->capacity) {
        vec->capacity *= 2;
        vec->entries = realloc(vec->entries, vec->capacity * sizeof(Entry));
    }
    vec->entries[vec->size].index = index;
    vec->entries[vec->size].value = value;
    vec->size++;
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
