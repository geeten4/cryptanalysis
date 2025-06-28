#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdint.h>
#include "blist.h"


void BasisList_init(BasisList *list, size_t initial_capacity) {
    list->data = malloc(initial_capacity * sizeof(fb_t));
    list->size = 0;
    list->capacity = initial_capacity;
}


void BasisList_append(BasisList *list, fb_t value) {
    if (list->size == list->capacity) {
        // Need to grow
        size_t new_capacity = list->capacity * 2;
        fb_t *new_data = realloc(list->data, new_capacity * sizeof(fb_t));
        list->data = new_data;
        list->capacity = new_capacity;
    }

    list->data[list->size++] = value;
}


fb_t BasisList_get(BasisList *list, size_t index) {
    if (index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    return list->data[index];
}

void BasisList_free(BasisList *list) {
    free(list->data);
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}

void BasisList_print(BasisList *list) {
    for (size_t i = 0; i < list->size; i++)
    {
        printf("%d, ", BasisList_get(list, i));
    }
    printf("\n");
}