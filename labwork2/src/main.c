#include <assert.h>
#include <stdbool.h> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>


// S substitution
uint16_t S[] = {0xf, 0xe, 0xb, 0xc, 0x6, 0xd, 0x7, 0x8, 0x0, 0x3, 0x9, 0xa, 0x4, 0x2, 0x1, 0x5};
//                  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   a,   b,   c,   d,   e,   f

// S inverse
uint16_t invS[] = {0x8, 0xe, 0xd, 0x9, 0xc, 0xf, 0x4, 0x6, 0x7, 0xa, 0xb, 0x2, 0x3, 0x5, 0x1, 0x0};
//                     0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   a,   b,   c,   d,   e,   f

// P permutation
uint16_t P[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
//                  0, 1, 2, 3,  4, 5, 6, 7,  8, 9, 10, 11, 12,13,14, 15
// P inverse
uint16_t invP[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};


uint16_t *shortIntToFourBlocks(uint16_t x) {
    /*
        given a (16 bit) short int x, decompose it into four 4-bit blocks, each
        given in a separate short int, in a 4-element array
    */
   uint16_t *A = malloc(4 * sizeof(short int));
   for (int i = 0; i < 4; i++)
        A[i] = (x >> 4*(3 - i)) & 15;

   return A;
}

uint16_t shortIntSBox(uint16_t a) {
    /*
        given a = A[0] || A[1] || A[2] || A[3]
        returns u = S[A[0]] || S[A[1]] || S[A[2]] || S[A[3]]
    */
   
    // divide into blocks
   uint16_t *A = shortIntToFourBlocks(a);
    
   // combine back together
   return (S[A[0]] << 12) | (S[A[1]] << 8) | (S[A[2]] << 4) | S[A[3]];
}

uint16_t shortIntInvSBox(uint16_t u) {
    /*
        given u = U[0] || U[1] || U[2] || U[3]
        returns a = invS[U[0]] || invS[U[1]] || invS[U[2]] || invS[U[3]]
    */

    // divide into blocks
   uint16_t *U = shortIntToFourBlocks(u);
    
   // combine bUck together
   return (invS[U[0]] << 12) | (invS[U[1]] << 8) | (invS[U[2]] << 4) | invS[U[3]];
}

uint16_t *generateRoundKeys(int numberOfKeys) {
    /*
        returns ptr to array of numberOfKeys randomly generated uint16_ts
    */
    uint16_t *k = malloc(numberOfKeys * sizeof(short int));
    for (int i = 0; i < numberOfKeys; i++)
    {
        k[i] = (short) rand();
    }
    return k;
}

uint16_t bitPermute(uint16_t x) {
    /*
    * permutes the bits of x according to P
    */
    uint16_t result = 0;
    for (int i = 15; i >= 0; i--)
    { 
        result <<= 1;
        if ( x & (1 << invP[i]) )
            result |= 1;
    }

    return result;
}

uint16_t inverseBitPermute(uint16_t x) {
    /*
    * permutes the bits of x according to P inverse
    */
    uint16_t result = 0;
    for (int i = 15; i >= 0; i--)
    { 
        result <<= 1;
        if ( x & (1 << P[i]) )
            result |= 1;
    }

    return result;
}

uint16_t encryptCipherD(uint16_t message, int roundCount, uint16_t *keys, bool lastRound) {
    /*
        given message as int, int roundCount as number of rounds to be iterated over
        and roundCount number of keys, returns encrypted message using CipherFour
    */

   uint16_t u = message, a, *A;

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

uint16_t decryptCipherD(int cypherText, int roundCount, uint16_t *keys) {
    uint16_t u = cypherText ^ keys[roundCount], *A = shortIntToFourBlocks(u);

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

void printBytes(uint16_t x) {
    for (int i = sizeof(short int) * 8 - 1; i >= 0 ; i--)
    {
        if ((x >> i) % 2 == 0)
            printf("%d", 0);
        else
            printf("%d", 1);

        if (i % 4 == 0) printf(" ");
    }
}

int xor_bits(unsigned x) {
    return __builtin_parity(x);
}

int arrayMaxValIndex(size_t *array, int length) {
    int maxIndex = 0;
    for (size_t i = 0; i < length; i++)
    {
        if (array[i] > array[maxIndex])
            maxIndex = i;
    }

    return maxIndex;
}

void firstExercise() {
    /*
        Implement encryption and decryption by CipherD defined in [3, p. 137– 139]. (Please do both 4 and 5 rounds.)
    */

    // Labwork 2. (i)
    printf("Labwork #2 (i): Implement encryption and decryption byCipherD:\n");

    uint16_t *keys, message, cypher;
    int numberOfRounds = 5;
    keys = generateRoundKeys(numberOfRounds + 1);
    message = 123;
    printf("Message to encrypt: %d\n", message);
    cypher = encryptCipherD(message, numberOfRounds, keys, true);
    printf("Encrypted message: %d\n", cypher);
    printf("Encrypted message: %d\n", decryptCipherD(cypher, numberOfRounds, keys));
}

void secondExercise() {
    /*
        Implement linear attackdescribed in [3, p. 140–142].You need to extract four bits of the last round key.
        
        Identical to experiment in Fig. 7.7 []
    */

    // Labwork 2. (ii)
    printf("Labwork #2 (ii): Implement linear attack:\n");


    // use the (iterative) linear characteristic (8 0 0 0) -R-> (8 0 0 0)
    uint16_t *keys, m, y, a, mask = (uint16_t ) 32768;

    int numberOfRounds = 5, randomMessageCount = 1000, experimentCount = 100;
    size_t guessedCorrectly = 0;
    for (size_t experimentCounter = 0; experimentCounter < experimentCount; experimentCounter++)
    {
        // initialize counters for the last key
        // 16 counters for a_3, where k_last = (a_3, a_2, a_1, a_0)
        size_t lastKeyCounters[16] = {0};

        keys = generateRoundKeys(numberOfRounds + 1); 

        for (size_t i = 0; i < randomMessageCount; i++)
        {
            // random message
            m = (uint16_t) rand();

            // encrypt
            y = encryptCipherD(m, numberOfRounds, keys, true);

            for (size_t lastKeyNibble = 0; lastKeyNibble < 16; lastKeyNibble++)
            {
                // work the round backwards
                a = shortIntInvSBox(y ^ (lastKeyNibble << 12));
                if(xor_bits(mask & m) == xor_bits(mask & a))
                    lastKeyCounters[lastKeyNibble]++;
            }
        }
        
        if (arrayMaxValIndex(lastKeyCounters, 16) == (keys[numberOfRounds] >> 12))
            guessedCorrectly++;
    }

    printf("First nibble of the last key had the highest bias %ld times out of %d experiments.\n", guessedCorrectly, experimentCount);
}

int main() {
    srand(time(NULL));   // Initialization, should only be called once.

    firstExercise();
    printf("\n");

    secondExercise();

    return 0;
}