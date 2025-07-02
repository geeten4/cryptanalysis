#include "utils.h"
#include "gf.h"
#include "blist.h"


void factor(BasisList* bl, gf_t n) {
    // factors n and stores factors into bl
    if (n <= 1) {
        printf("No factors for %d\n", n);
        return;
    }

    // Factor out 2's
    while (n % 2 == 0) {
        BasisList_append(bl, 2);
        n /= 2;
    }

    // Factor odd numbers up to sqrt(n)
    gf_t limit = (gf_t) sqrt(n);
    for (gf_t i = 3; i <= limit && n > 1; i += 2) {
        while (n % i == 0) {
            BasisList_append(bl, i);
            n /= i;
            limit = (gf_t) sqrt(n);
        }
    }

    // If remaining n is prime and > 2
    if (n > 2) BasisList_append(bl, n);
}