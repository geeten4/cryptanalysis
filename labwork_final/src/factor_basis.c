#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include "blist.h"
#include "factor_basis.h"


BasisList *create_factor_basis(fb_t B) {
    /*
        return list of prime numbers up to the number B inside a BasisList
    */
    BasisList *bl = malloc(sizeof(BasisList));
    BasisList_init(bl, 10);

    // initial prime being 2
    BasisList_append(bl, 2);

    bool is_prime = true;
    fb_t test_prime;
    size_t j = 0;
    // create a simple sieve to return primes up to B
    for (size_t i = 3; i < B; i++)
    {
        while(j < bl->size) {
            test_prime = BasisList_get(bl, j);
            if((i % test_prime) == 0) {
                is_prime = false;
                break;
            } else if (test_prime * test_prime > i) {
                break;
            }
            j++;
        }

        if (is_prime)
            BasisList_append(bl, i);

        is_prime = true;
        j = 0;
    }

    return bl;
}

