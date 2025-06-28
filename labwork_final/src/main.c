#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include "factor_basis.h"
#include "blist.h"
#include "vector.h"
#include <math.h>
#include "gf.h"


int main() {
    BasisList *fb = create_factor_basis(100000);
    // BasisList_print(fb);

    int i = 1232;
    Vector *v = malloc(sizeof(Vector));
    Vector_init(v);  
    
    Vector_express_in_fbasis(v, fb, i);
    Vector_print(v);
    
    Vector_scale(v, 10, 7);
    Vector_print(v);
    Vector_scale(v, 10, 7);
    Vector_print(v);

    printf("%d\n", mod_add(2, 8, 7));
}