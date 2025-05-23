#ifndef SET_H
#define SET_H

#include "gf.h"

typedef struct {
    gf2_11 *data;
    int size;
    int capacity;
} Set;

Set* create_set();
void set_free(Set *set);
int set_contains(Set *set, gf2_11 x);
void set_add(Set *set, gf2_11 x);
void set_remove(Set *set, gf2_11 x);
void set_print(Set *set);
void set_subtract(Set *set1, Set *set2);

#endif