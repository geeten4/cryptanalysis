#ifndef BLIST_H
#define BLIST_H
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <stddef.h>

typedef uint64_t fb_t;

typedef struct {
    fb_t *data;
    size_t size;
    size_t capacity;
} BasisList;

BasisList* BasisList_create(size_t initial_capacity);
void BasisList_init(BasisList *list, size_t initial_capacity);
void BasisList_append(BasisList *list, fb_t value);
fb_t BasisList_pop(BasisList* list);
void BasisList_set(BasisList *list, size_t index, fb_t value);
fb_t BasisList_get(BasisList *list, size_t index);
void BasisList_free(BasisList *list);
void BasisList_print(BasisList *list);

#endif  // BLIST_H