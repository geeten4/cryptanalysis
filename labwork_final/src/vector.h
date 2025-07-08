#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdio.h>
#include "gf.h"

#define INIT_CAPACITY 10;

typedef struct {
    int index;
    gf_t value;
} Entry;

typedef struct {
    Entry* entries;
    int size;     
    int capacity; 
    int length;   
} Vector;

// basic 
Vector* vector_create(int length);
int vector_set(Vector* vec, int index, gf_t value);
gf_t vector_get(const Vector* vec, int index);
void vector_increment(Vector* vec, int index);
void vector_set_zeros(Vector* vec);
void vector_free(Vector* vec);
void vector_print(const Vector* vec);
void vector_print_with_zeros(const Vector* vec);

// linear operations
void vector_scale(Vector* vec, gf_t scalar, gf_t p);
void vector_add_scaled(Vector* vec, const Vector* other, gf_t scalar, gf_t p);
void vector_mod(Vector* vec, gf_t p);

// factor basis operations
gf_t vector_express_in_fbasis(Vector* vec, BasisList* fb, gf_t n);
gf_t vector_multiply_in_fbasis(Vector* vec, BasisList* fb);

// special operations
void crt_combine_vectors(Vector** vectors, BasisList* moduli, int n, Vector* v);
gf_t* extended_euclidian_algorithm(gf_t a, gf_t b);
gf_t vector_dot_product_mod(Vector* v, Vector* w, gf_t p);

#endif // VECTOR_H
