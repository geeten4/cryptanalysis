#include <assert.h>
#include <stdbool.h> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


// S substitution
unsigned int S[] = {0xf, 0xe, 0xb, 0xc, 0x6, 0xd, 0x7, 0x8, 0x0, 0x3, 0x9, 0xa, 0x4, 0x2, 0x1, 0x5};
//                  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   a,   b,   c,   d,   e,   f

// S inverse
unsigned int invS[] = {0x8, 0xe, 0xd, 0x9, 0xc, 0xf, 0x4, 0x6, 0x7, 0xa, 0xb, 0x2, 0x3, 0x5, 0x1, 0x0};
//                     0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   a,   b,   c,   d,   e,   f

// P permutation
unsigned int P[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
//                  0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11, 12,13,14, 15
// P inverse
unsigned int invP[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};


unsigned short int *shortIntToFourBlocks(unsigned short int x) {
    /*
        given a (16 bit) short int x, decompose it into four 4-bit blocks, each
        given in a separate short int, in a 4-element array
    */
   unsigned short int *A = malloc(4 * sizeof(short int));
   for (int i = 0; i < 4; i++)
   {
        A[i] = (x >> 4*(3 - i)) & 15;
   }
   return A;
   
}

unsigned short int shortIntSBox(unsigned short int a) {
    /*
        given a = A[0] || A[1] || A[2] || A[3]
        returns u = S[A[0]] || S[A[1]] || S[A[2]] || S[A[3]]
    */
   
    // divide into blocks
   unsigned short int *A = shortIntToFourBlocks(a);
    
   // combine back together
   return (S[A[0]] << 12) | (S[A[1]] << 8) | (S[A[2]] << 4) | S[A[3]];
}

unsigned short int shortIntInvSBox(unsigned short int u) {
    /*
        given u = U[0] || U[1] || U[2] || U[3]
        returns a = invS[U[0]] || invS[U[1]] || invS[U[2]] || invS[U[3]]
    */

    // divide into blocks
   unsigned short int *U = shortIntToFourBlocks(u);
    
   // combine bUck together
   return (invS[U[0]] << 12) | (invS[U[1]] << 8) | (invS[U[2]] << 4) | invS[U[3]];
}

unsigned short int *generateRoundKeys(int numberOfKeys) {
    /*
        returns ptr to array of numberOfKeys randomly generated unsigned short ints
    */
    unsigned short int *k = malloc(numberOfKeys * sizeof(short int));
    for (int i = 0; i < numberOfKeys; i++)
    {
        k[i] = (short) rand();
    }
    return k;
}

unsigned short int bitPermute(unsigned short int x) {
    /*
    * permutes the bits of x according to P
    */
    unsigned short int result = 0;
    for (int i = 15; i >= 0; i--)
    { 
        result <<= 1;
        if ( x & (1 << invP[i]) )
            result |= 1;
    }
    return result;
}

unsigned short int inverseBitPermute(unsigned short int x) {
    /*
    * permutes the bits of x according to P inverse
    */
    unsigned short int result = 0;
    for (int i = 15; i >= 0; i--)
    { 
        result <<= 1;
        if ( x & (1 << P[i]) )
            result |= 1;
    }
    return result;
}

unsigned short int encryptCipherD(unsigned short int message, int roundCount, unsigned short int *keys, bool lastRound) {
    /*
        given message as int, int roundCount as number of rounds to be iterated over
        and roundCount number of keys, returns encrypted message using CipherFour
    */

   unsigned short int u = message, a, *A;

   // go through rounds
    for (int i = 0; i < roundCount - 1; i++)
    {
        a = u ^ keys[i];
    
        // divide into blocks
        A = shortIntToFourBlocks(a);
    
        // combine back together
        u = (S[A[0]] << 12) | (S[A[1]] << 8) | (S[A[2]] << 4) | S[A[3]];
    
        // permute according to P
        u = bitPermute(u);
    }

    // return without last round
    if (!lastRound) {
        return u;
    }

    // last round
    a = u ^ keys[roundCount - 1];

    // divide into blocks
    A = shortIntToFourBlocks(a);

    // combine back together
    u = (S[A[0]] << 12) | (S[A[1]] << 8) | (S[A[2]] << 4) | S[A[3]];

    // last xor
    return u ^ keys[roundCount];
}

unsigned short int decryptCipherD(int cypherText, int roundCount, unsigned short int *keys) {
    unsigned short int u = cypherText ^ keys[roundCount], *A = shortIntToFourBlocks(u);

    u = (invS[A[0]] << 12) | (invS[A[1]] << 8) | (invS[A[2]] << 4) | invS[A[3]];
    u ^= keys[roundCount - 1];

    for (int i = roundCount - 2; i >= 0 ; i--)
    {
        u = inverseBitPermute(u);
        A = shortIntToFourBlocks(u);
        u = (invS[A[0]] << 12) | (invS[A[1]] << 8) | (invS[A[2]] << 4) | invS[A[3]];
        u ^= keys[i];
    }
    
    return u;
}

void printBytes(unsigned short int x) {
    for (int i = sizeof(short int) * 8 - 1; i >= 0 ; i--)
    {
        if ((x >> i) % 2 == 0) {
            printf("%d", 0);
        } else {
            printf("%d", 1);
        }
        if (i % 4 == 0) {
            printf(" ");
        }
    }
}

void main() {
    unsigned short int *keys, message, cypher;
    int numberOfRounds = 5;
    keys = generateRoundKeys(numberOfRounds);
    message = 123;
    cypher = encryptCipherD(message, numberOfRounds, keys, true);
    printf("decrypted: %d\n", decryptCipherD(cypher, numberOfRounds, keys));
}