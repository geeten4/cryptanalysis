#ifndef BINDEX_H
#define BINDEX_H

#include <stddef.h>
#include "factor_basis.h"
#include "gf.h"

typedef struct VectorNode
{
    fb_t value;
    int index;
    struct VectorNode* left;
    struct VectorNode* right;
} VectorNode;

typedef struct
{
    VectorNode *head;
} Vector;


// basic vector manipulation
void Vector_init(Vector *v);
void Vectore_free(Vector *v);
void Vector_print(Vector *v);
int Vector_insert_value(Vector *v, int index, fb_t value);
fb_t Vector_get_value(Vector *v, int index);
void Vector_increment(Vector *v, int index);
int Vector_next_nonzero_index(Vector *v, int index);

// factor basis
fb_t Vector_express_in_fbasis(Vector *v, BasisList *fb, fb_t n);
fb_t Vector_multiply_with_fbasis(Vector *v, BasisList *fb);

// linear manipulation
void Vector_scale(Vector *v, fb_t c, gf_t p);  // multiply v by c modulo p
void Vector_add_vector_multiple(Vector *v, fb_t c, Vector *w, gf_t p);  // to v add c*w modulo p

#endif  // BINDEX_H