#ifndef SET_H
#define SET_H

#include <stdint.h>

typedef uint16_t set_arg_t;

typedef struct {
    set_arg_t *data;
    int size;
    int capacity;
} Set;

Set* create_set();
void set_free(Set *set);
int set_contains(Set *set, set_arg_t x);
void set_add(Set *set, set_arg_t x);
void set_remove(Set *set, set_arg_t x);
void set_print(Set *set);
void set_subtract(Set *set1, Set *set2);

#endif