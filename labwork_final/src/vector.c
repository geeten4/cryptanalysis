#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "vector.h"


void Vector_init(Vector *v) {
    v->head = 0;
}

int Vector_insert_value_recursive(VectorNode *v, VectorNode *node) {
    if ((v->index < node->index) & (v->right != 0))
        return Vector_insert_value_recursive(v->right, node);
    else if ((v->index < node->index) & (v->right == 0))
        v->right = node;
    else if ((v->index > node->index) & (v->left != 0))
        return Vector_insert_value_recursive(v->left, node);
    else if ((v->index > node->index) & (v->left == 0))
        v->left = node;
    else {
        v->value = node->value;
        free(node);
    }
        
    return 0;
}

int Vector_insert_value(Vector *v, int index, fb_t value) {
    VectorNode *newNode = malloc(sizeof(VectorNode));
    newNode->value = value;
    newNode->index = index;
    newNode->left = 0;
    newNode->right = 0;

    if (v->head == 0) {
        v->head = newNode;
        return 0;
    } else {
        return Vector_insert_value_recursive(v->head, newNode);
    }
}

void Vector_print_recursive(VectorNode *node) {
    if(node->left != 0) {
        Vector_print_recursive(node->left);
    }
    printf("%d: %d, ", node->index, node->value);
    if (node->right != 0) {
        Vector_print_recursive(node->right);
    }
}

void Vector_print(Vector *v) {
    printf("(");
    if(v->head != 0)
        Vector_print_recursive(v->head);
    printf(")\n");
}

void Vectore_free_recursive(VectorNode *node) {
    if (node->left != 0) {
        Vectore_free_recursive(node->left);
        free(node->left);
    }
    
    if (node->right != 0) {
        Vectore_free_recursive(node->right);
        free(node->right);
    }
}

void Vectore_free(Vector *v) {
    if(v->head != 0)
        Vectore_free_recursive(v->head);
    free(v);
}

fb_t Vector_get_index_recursive(VectorNode *node, int index) {
    if ((node->index < index) & (node->right != 0))
        return Vector_get_index_recursive(node->right, index);
    else if ((node->index < index) & (node->right == 0))
        return 0;
    else if ((node->index > index) & (node->left != 0))
        return Vector_get_index_recursive(node->left, index);
    else if ((node->index > index) & (node->left == 0))
        return 0;
    else {
        return node->value;
    }
}

fb_t Vector_get_value(Vector *v, int index) {
    if (v->head == 0)
        return 0;
    return Vector_get_index_recursive(v->head, index);
}

void Vector_increment(Vector *v, int index) {
    Vector_insert_value(v, index, Vector_get_value(v, index) + 1);
}

int Vector_next_nonzero_index_recursive(VectorNode *node, int index, int current_minimum) {
    if ((node->index <= index) & (node->right != 0))
        return Vector_next_nonzero_index_recursive(node->right, index, current_minimum);
    else if ((node->index <= index) & (node->right == 0))
        return -1;
    else if ((node->index > index) & (node->left != 0))
        return Vector_next_nonzero_index_recursive(node->left, index, node->index);
    else if ((node->index > index) & (node->left == 0)) {
        return node->index;
    }
    else 
        return -1;
}

int Vector_next_nonzero_index(Vector *v, int index) {
    // returns the position of the next non-zero entry strictly greater than index
    // if no such is found, returns -1
    if(v->head == 0)
        return -1;
    return Vector_next_nonzero_index_recursive(v->head, index, -1);
}

fb_t Vector_express_in_fbasis(Vector *v, BasisList *fb, fb_t n) {
    fb_t b, m = n;

    for (size_t i = 0; i < fb->size; i++)
    {
        b = BasisList_get(fb, i);
        while((m % b == 0) && (m > 1)) {
            m /= b;
            Vector_increment(v, i);
        }
    }

    return m;
}

fb_t Vector_multiply_with_fbasis(Vector *v, BasisList *fb) {
// given a factor basis fb and a basis vector bv,
    // multiply the factor basis given exponents in bv
    // i.e. inverse to Vector_express
    fb_t n = 1;
    int i = Vector_next_nonzero_index(v, -1);
    while (i != -1)
    {
        n *= pow(BasisList_get(fb, i), Vector_get_value(v, i));
        i = Vector_next_nonzero_index(v, i);
    }
    
    return n;
}

void Vector_scale_recursive(VectorNode *node, fb_t c, gf_t p) {
    node->value = mod_mul(node->value, c, p);
    if(node->left != 0) {
        Vector_scale_recursive(node->left, c, p);
    }
    if (node->right != 0) {
        Vector_scale_recursive(node->right, c, p);
    }
}

void Vector_scale(Vector *v, fb_t c, gf_t p) {
    if(v->head != 0) {
        Vector_scale_recursive(v->head, c, p);
    }
}

void Vector_add_vector_multiple_recursive(VectorNode *node, fb_t c, Vector *w, gf_t p) {
    if ((v->index < node->index) & (v->right != 0))
        return Vector_insert_value_recursive(v->right, node);
    else if ((v->index < node->index) & (v->right == 0))
        v->right = node;
    else if ((v->index > node->index) & (v->left != 0))
        return Vector_insert_value_recursive(v->left, node);
    else if ((v->index > node->index) & (v->left == 0))
        v->left = node;
    else {
        v->value = node->value;
        free(node);
    }
        
    return 0;
}

void Vector_add_vector_multiple(Vector *v, fb_t c, Vector *w, gf_t p) {
    // to v add c*w modulo p
    if(w->head == 0) return;

    if(v->head == 0)
}