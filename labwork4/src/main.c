#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "permutation.h"

int main() {
    printf("%d\n", permutation_inverse(permutation(1234)));
    return 0;
}
